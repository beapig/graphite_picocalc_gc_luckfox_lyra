# 13. What went wrong

Every other chapter in this guide describes code that works. This one describes
four bugs, in enough detail to follow how each was found, and it is the chapter
worth reading first.

The reason is not modesty. Three of these four could not have been found by
reading the code — they needed a board, a measurement, or a test that nobody had
thought to write. Two of them had shipped for months. One of them is still open
as this is written. And in each case the *wrong* first explanation is as
instructive as the right one, so the wrong ones are here too.

None of this is unique to calculators. A 4 KB stack, two cores and a memory bus
you have to talk to over SPI just make the consequences arrive faster.

---

## 1. The editor that froze, and the renderer that compiled

**Symptom.** From the bench: *"the Y= function editor freezes the calculator,
every time, on Pico 1 — only the first few pixel rows of the header render, then
keys are dead and it needs a physical power cycle."*

Every part of that description turned out to matter.

**What was happening.** The slot editor coloured each row red or white depending
on whether the expression in it parsed. It did that by calling `field_valid()`,
which called `Engine::compile()` — **from inside `render()`**.

`Engine::compile` had a 2,232-byte stack frame. Almost all of it was one local:

```c
te_variable lookup[kLookupCount];   // 122 entries x 16 B = 1,952 B
```

a table of every variable and function name, rebuilt on the stack on every
single compile. Core 0's stack is 4 KB total.

**Why the symptoms were exactly those symptoms.** The display renders in 16-pixel
horizontal strips. The header bar is exactly 16 pixels tall — one strip. So strip
0 pushed fine. Core 0 then began rendering strip 1 *while core 1 was still
DMA-ing strip 0 to the panel*, blew past the bottom of its own stack, and landed
in core 1's live frames. Core 1 died mid-transfer. Core 0 then blocked forever
waiting for an acknowledgement that would never come — and since keys are polled
on core 0, input died with it.

"Only the first few pixel rows render" was not a clue about the renderer. It was
the stack overflow, reported in pixels.

**Why it didn't fault.** This is the part worth internalising. On the RP2040 the
two cores' stacks live in adjacent 4 KB scratch banks: core 0's sits directly on
top of core 1's. Running off the bottom of one is not an invalid access — it is a
perfectly valid write to memory that belongs to something else. There is no
trap, no fault, no diagnostic. The program keeps running and the corruption
surfaces somewhere unrelated, later.

**The fix, and the fix behind the fix.** Two changes, and only the second one is
interesting:

1. `render()` now only draws. Validity is computed when a key is pressed and
   cached in a bitfield. This is the contract the list editor had documented
   since Phase 3A; the slot editors had simply never been held to it.
2. The lookup table moved to `.bss` — built once at startup instead of per
   compile, because after startup it never changes. `Engine::compile` went from
   **2,232 bytes to 280**, everywhere in the firmware.

Change 1 alone would have fixed the reported bug. It would also have left a
2 KB stack frame sitting under a function called twenty times a frame, waiting
for the next caller to trip over it.

**The measurement that found it.** The root cause was not found by reading
`render()`. It was found by building a tool that walks function prologues out of
the linked ELF and reports every frame over a threshold. Then the frame report
was read top to bottom — and immediately found a *second*, worse instance nobody
had reported:

```
HomeScreen::evaluate_input   872 B
  listexpr::evaluate       1,192 B
    eval_list_into         2,248 B  <-- and recursive
                          -------
                           4,312 B at recursion depth 1
```

A 4 KB stack. Typing `{1,2,3}` on the home screen — a path that had been
hardware-verified since Phase 3A — was already overrunning into core 1, silently,
and had been for months. `eval_list_into` went from 2,248 bytes per level to
**32**.

**The lesson.** Stack frames are a budget, and you cannot feel a budget by
reading code. Measure it, and read the whole report rather than the line you were
looking for.

---

## 2. The guard that was one frame too late

**Symptom.** `det(([a]*([c]+[d]))+[d])` hard-faults the device, reproducibly.

The matrix expression parser was the last of four parsers with no limit on how
deep it would recurse. The other three had all been given one — and three of the
four limits had been discovered by something crashing first.

**The first fix.** Add a depth cap. Count nesting levels, refuse anything past
the limit, return "Too deeply nested" instead of recursing. The counter is an
RAII guard placed in `parse_unary`, the one function every level passes through
exactly once:

```cpp
static bool parse_unary(State& s, Value& out) {
    DepthGuard guard(s);          // increments; decrements on scope exit
    if (guard.too_deep()) {
        return error(s, "Too deeply nested");
    }
    ...
}
```

Picking the limit took measurement, not arithmetic. Frame sizes suggested depth 3
would need ~4,300 bytes and was therefore unreachable, so the cap was first set
to 2 — which broke two behaviours the test suite already pinned. On real hardware
depth 3 turned out to *fit*, at 3,940 bytes of 4,096. The static arithmetic had
been 360 bytes pessimistic, because the frames are not all live simultaneously.

Cap set to 3. Verified on hardware. The hard fault became an error message. This
was written up, correctly, as **containment rather than a fix** — it left 84
bytes of margin.

**Why it was still wrong.** Months later, a different board ran
`det((([A]*[A])+[A])*[A])` — depth 4, which the cap explicitly rejects. The
device rebooted.

Look at the guard again. `DepthGuard` is a local variable *inside*
`parse_unary`. To construct it, you must first enter the function. To enter the
function, the compiler must allocate its frame — all ~600 bytes of it.

**The frame is allocated before the guard can refuse it.** With 144 bytes of
margin and a 600-byte frame, the last permitted level overruns the stack while
being told it is not permitted. The guard was one frame too late, and had been
from the day it was written.

The check has to happen *before* the recursive call, at each call site — not on
arrival, inside the callee. It is a two-line difference and it is the whole
difference.

**Why nobody caught it.** The second board's frames are about 12% smaller and its
idle baseline lower, so it survives the input that kills the first. The
verification sequence had gone board-one-then-board-two; the input that would
have exposed the flaw was only ever tried on the board that tolerates it.

**The method note, which outlived the bug.** Three separate attempts to predict a
stack peak from frame sizes were made during this work. All three were wrong, and
all three were wrong in the same direction — optimistic. Depth 3 was predicted
unreachable and fit. One board was predicted at ~3,500 and crashed. The same
board post-fix was predicted at ~3,300 and measured 3,860.

> Static frame sums bound a *single frame*. They are not a model of a peak.

Where a board is not available, prefer a **monotonic argument** — "this change
can only ever remove stack, so a previously-passing case cannot regress" — over a
prediction. A monotonic argument survives being wrong about the magnitude.

---

## 3. Two implementations of the same thing, quietly disagreeing

**Symptom.** From the bench: *"trig functions in degree mode give results
identical to radian mode."*

**What was happening.** There were two evaluators. The real one handled ordinary
arithmetic; the complex one took over whenever the number mode was `a+bi` or
`r∠θ`. The real evaluator routed trig through a helper that applied degree
scaling. The complex evaluator called `std::sin` on the raw value.

So in `a+bi` mode, **every trig call answered in radians and the MODE screen's
DEGREE setting was silently ignored.** `sin(30)` gave `0.5` on one path and
`-0.988...` on the other.

It had shipped that way for months, across several releases.

**Why no test caught it.** This is the useful part. There were tests for complex
arithmetic. There were tests for angle modes. There were tests for exact trig
values.

Not one of them crossed the two. The complex tests never varied angle mode; the
angle-mode tests only ever ran through the real evaluator. Every individual
behaviour was covered and the *relationship between them* was not covered at all,
because no single test file owned it.

The fix included a test that asserts the two evaluators agree on a corpus of
real-valued trig in both modes. That property — **for a real-valued argument, the
two paths must produce identical answers** — is the invariant that had been
violated, and nothing had been asserting it.

**A second, subtler instance found the same day.** Certain results displayed in
white in REAL mode but amber in `a+bi` mode. Amber means "this has an exact form
worth showing".

The cause: complex exponentiation was unconditionally `exp(ln(base) * exp)`, so
`10202^2` came back a hair off the integer — enough to fail an
`x == floor(x)` check, which dropped it out of the integer print branch into
`%.10g`, which printed a fractional digit, which made the exact-form checker
notice a discrepancy, which coloured it amber.

Only one test value in the whole ladder showed it, and the reason is worth
recording:

- Smaller values: the drift never reaches the tenth significant digit.
- Values past $10^{10}$: printed in scientific notation, where it is invisible.
- **104080805 is nine digits — the only rung whose tenth significant digit lands
  in the fractional part.**

Change the test data slightly and this bug is invisible. It was found by a ladder
of increasing magnitudes, which is exactly the shape of test that catches
precision faults and exactly the shape nobody writes when testing a feature.

Note also what was *not* broken: the amber was the exact-form feature working
correctly. The decimal really had drifted; the exact value really was the better
answer. The defect was upstream in the arithmetic, and the feature that surfaced
it was doing its job.

**The lesson.** Two implementations of the same operation will diverge. Not
might — will. If your design has two, something must continuously assert they
agree, and that assertion belongs to neither of them.

---

## 4. The read that fails two percent of the time, unless you are watching

This one is **still open**. It is here because an unsolved bug honestly described
teaches more than a solved one tidied up afterwards.

**Symptom.** A 999-element list, divided by a constant and displayed, renders
element 1 as `4.004007642e-6` instead of `4.004004004e-6` — on roughly **8 runs
in 30**. Same board, same session, same input. On one of the two boards only.

It had shipped. It was found because a verification script replayed the same
expression thirty times in a row, which no human bench pass had ever done. Typed
once, it looks correct 73% of the time.

### Two wrong answers first

Before anything true was established, two tests were run and both were
misinterpreted — in the same way.

**Test 1:** `sum(l1*1)` returns `499500` on 20 out of 20 runs. Conclusion drawn:
the values are fine, only the display is broken.

That conclusion was unsupported. An element error of 1.8e-6 moves a sum of
499,500 to 499500.0000018 — which formats **identically** at ten significant
figures. The test could not have failed.

**Test 2:** `sum(l1/499500)` returns exactly `1` on 25 of 25. Read as
confirmation. Worse than the first: after the division the element error is
3.6e-12. The operation intended to expose the fault had buried it four orders
deeper.

**Test 3:** `sum(l1) - 499500` returns `0` on 25 of 25. *This* one can see it —
subtracting the expected total leaves the residue with nothing to hide behind.

> A sum is the wrong instrument for a single-element fault.

Reaching for one twice cost more than the bug did. When you fold data down to one
number, ask what magnitude of error that number can still show — before you read
anything into the result.

### What is actually known

With folds that can see a single-element error (`sum(x) - expected`, never
`sum(x)` alone), three of the four candidate locations were eliminated:

| Candidate | Test | Result |
|---|---|---|
| The write path | all 999 elements, several forms | 0 failures in 30 |
| The stored data | round-trip through the failing expression | 0 in 30, stable to the digit |
| Bulk reads | in-firmware control | 0 in 8,000 |

So the fault is in the **per-element read** and nowhere else in the chain.
Bulk reads stream in chunks and are clean; the display path fetched one 8-byte
value at a time, and is not.

### The finding that killed the best theory

The obvious explanation was contention. Memory is reached over a PIO-driven SPI
bus using DMA, and the other core is pushing the display over DMA at the same
time — and the board that fails pushes in twenty small strips per frame where the
board that doesn't pushes one large buffer. More concurrent DMA, on exactly the
board with the defect. Everything fit.

So a diagnostic build was written to hammer per-element reads directly, in six
variants: settled reads, reads immediately after a bulk write, reads interleaved
with writes elsewhere, spaced reads, the display formatter itself, and a bulk
control.

**40,000+ per-element reads. Zero failures.** Against roughly 2% in the real
path.

That is a **thousandfold** discrepancy, and it is decisive in a direction nobody
wanted: the trigger is *contextual*, not a random per-read fault. Any theory
predicting a uniform error rate is wrong — including the contention theory, which
was the good one.

A negative result that eliminates your best hypothesis is worth more than a
positive result confirming it. This one cost a day and was the most valuable
output of the investigation.

### The signature was misread too

An early note described a partial transfer — "the low four bytes didn't land".
That came from comparing bit patterns of a *derived* quantity, computed from the
displayed value, rather than the value the read actually returned.

Comparing what was really read gives a **single-bit flip at mantissa bit ~32**.
Not a partial transfer. Different bug, different cause, different fix.

Compounding it: the display carries only ten significant figures, so the low
mantissa bytes were never present in the numbers being reasoned about. The
byte-lane analysis had been built on digits that did not exist.

> Reason about the bytes you measured, not the bytes you derived from a
> rendering.

### Where it stands

The display path now reads in blocks, mirroring the compute path. Verified: **0
corrupted in ~144 evaluations**, against 8/30 before. And the control that is
usually skipped was run — reverting the fix brings the fault straight back, 7/30
and 2/30, with the same corrupt value. The fix is causal, not coincidence.

But that is a symptom fix and it is labelled as one. The root cause is unknown,
the entry stays open, and roughly seventeen other per-element read sites carry
the same theoretical exposure — they have simply never been hammered thirty times
in a row.

The one difference never eliminated: the diagnostic allocated its own array,
while the real failure formats a temporary from the evaluator's pool — a
different memory region, reached by a different allocation path. **The next step
is to log the failing temporary's address**, not to test the driver again.

It would have been easy to fix the display path, watch the symptom vanish, and
close the bug. The symptom is what a user sees; the cause is what bites the next
person.

---

## What these have in common

**Three of four could not be found by reading code.** One needed a frame-size
report generated from the linked binary. One needed a second physical board. One
needed the same input repeated thirty times. Code review would have caught none
of them — not because the reviewers were careless, but because the information
required was not in the source.

**Two had shipped for months.** Both on paths marked hardware-verified. The
verification was real; it just asked "does this work?" and got a truthful yes.
Neither asked "does this still work the hundredth time?" or "does it work on the
other board?"

**Every first explanation was wrong**, and usefully so. The freeze looked like a
rendering bug. The depth cap looked fixed for months. The list corruption looked
like a display artifact, then like DMA contention, then like a partial transfer.
Each wrong answer was reached honestly from the evidence available, and each was
discarded by a test designed to be able to say no.

**The tests that found things were shaped differently from the tests that passed.**
A ladder of magnitudes, not a value. The same input thirty times, not once. A
fold that can see a single-element error, not a sum. Both boards, not the
convenient one. An assertion that two implementations agree, owned by neither.

**And the fix and the defect are different things.** The frozen editor was fixed
by caching validity; the *defect* was a 2 KB frame under a renderer. The list
corruption was fixed by block reads; the defect is still out there. Ship the fix,
then be honest in writing about which one you shipped.

---

*Primary sources for this chapter are decision-log entries D46, D47, D48 and D53
in the repository, which record these as they were found — including the wrong
turns, undeleted.*
