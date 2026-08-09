# Feature wishlist — historical record

Features that were wanted but not scheduled into a phase. This file ran the
project's backlog from Phase 1 through 5.2; it is now the archive of what
graduated and what shipped, and GitHub Issues carries what is still open.

The workflow it used to describe — pull an item into a phase, move it to
**Graduated** with the target sub-phase, then to **Completed / Closed** with
the as-built D-number — now maps onto Issues directly: assigning a milestone
graduates an item, and closing it with a link to the decision entry completes
it. The point of both is the same and is worth restating: **keep the
provenance end to end**, so an item still makes sense to whoever reads it a
year later.

> **This file is closed to new entries (2026-08-10).** Open work now lives in
> [GitHub Issues](https://github.com/moodoki/graphite_picocalc_gc/issues); the
> ten items that were under *Active* moved there with their provenance intact
> (issues #14–#23). See [issue-tracking.md](issue-tracking.md) for what moved
> and what deliberately did not.
>
> The two sections below stay, and stay here rather than moving: they are the
> record of items that already graduated or shipped, cross-referenced by
> D-number, and GitHub would gain nothing by holding them.
>
> Run `./scripts/gh-issues.py` for a local mirror of the current backlog in
> this file's own format.

## Active (unscheduled) — migrated

Migrated to GitHub Issues on 2026-08-10. Each issue opens with the line
recording when the item was raised and what raised it, which was this
section's real value:

| Issue | Item |
|---|---|
| [#14](https://github.com/moodoki/graphite_picocalc_gc/issues/14) | Collapse `Kind::kMatrix` and `Kind::kList` into one shape-driven array kind |
| [#15](https://github.com/moodoki/graphite_picocalc_gc/issues/15) | Re-vendor tinyexpr from upstream `master` |
| [#16](https://github.com/moodoki/graphite_picocalc_gc/issues/16) | Replace tinyexpr with the unified evaluator on the numeric path |
| [#17](https://github.com/moodoki/graphite_picocalc_gc/issues/17) | Inverse-trig exact forms |
| [#18](https://github.com/moodoki/graphite_picocalc_gc/issues/18) | Say *why* an editor field is invalid |
| [#19](https://github.com/moodoki/graphite_picocalc_gc/issues/19) | Screenshot capture |
| [#20](https://github.com/moodoki/graphite_picocalc_gc/issues/20) | Crosshair in trace mode |
| [#21](https://github.com/moodoki/graphite_picocalc_gc/issues/21) | Copy/paste in expression editors |
| [#22](https://github.com/moodoki/graphite_picocalc_gc/issues/22) | 3D plotting |
| [#23](https://github.com/moodoki/graphite_picocalc_gc/issues/23) | Antialiased / higher-res font rendering (D31) |

#15 and #16 are alternatives to each other — whichever is decided first should
discharge the other.


## Graduated — now planned

- **Serial key injection for on-device test automation** (raised
  2026-08-05, Pico 1 testdrive) -> **Phase 5.1** (see
  [phase5.1-spec.md](../phases/phase5.1-spec.md), tasks 5.1.1-5.1.6),
  scoped 2026-08-08 to the line-oriented variant. Per-keystroke `KeyEvent`
  synthesis stays deferred with an explicit revival trigger (that spec's
  section 7). Two findings closed the gap between "idea" and "planned": the
  sibling screenshot item below is **not** a prerequisite, because
  `HomeScreen::ResultKind` (`home_screen.hpp:36`) already encodes
  white/amber/error and can simply be printed; and flashing no longer needs
  the BOOTSEL button (`picotool load -f -x`), leaving keyboard input as the
  last manual step in the bench loop. Motivated by D48, whose bench work
  needed ~15 hand round-trips to land one integer.
- **Pi-multiple axis ticks + `▶Frac`/`▶Dec` fraction answers** (split off
  the old "Symbolic display" item) → Phase 4, sub-phase **4D** (see
  [phase4-spec.md](../phases/phase4-spec.md) §7.1, tasks 4D.2/4D.3).
- **Surd / exact-value display** (the other split-off piece of the old
  "Symbolic display" item, raised Session 10 eval): keeping $\sqrt{2}$ as
  $\sqrt{2}$ instead of a decimal → folded into **Phase 5** core scope on
  2026-07-21 (see [phase5-spec.md](../phases/phase5-spec.md) §10.1, tasks
  4D.23/4D.24) — it needs the `Expr` tree and `simplify()` Phase 5
  builds anyway, so there was no reason to leave it homeless once Phase 5
  existed. Unit/dimensional arithmetic (`3 m/s` staying symbolic) is a
  materially bigger feature and remains explicitly out of scope (Phase 5
  non-goals, §13).
- **True subscripts** (`Sₓ`, `σₓ`) in stats/inference display (Session 13,
  D24.9; the piece D31's Greek-letter pass left open) → Phase 4 **4D**
  (§7.1, task 4D.4).
- **Vertical centering for fraction expressions** → Phase 4 **4D** (§7.1,
  task 4D.5).
- **Scientific constants** (Session 13, D24.9) → Phase 4 **4D** (§7.4,
  task 4D.17).
- **Unit conversions** (Session 13, D24.9) → Phase 4 **4D** (§7.4, task
  4D.18) as a native catalog, not a later app — closes the gap while it's
  cheap; TI-84 CE ships these natively too, not as a sideloaded app.
- **Beyond 6 lists** (Session 13, D24.9; the "named lists" half of the old
  combined item) → Phase 4 **4D** (§7.3, task 4D.13), capped by whatever
  `ArrayStore` headroom actually allows — see phase4-spec.md open question
  P4-10. *(The "SD list-data files / CBL-CBR data-logger" half of the old
  item did **not** graduate — still unscoped, no phase home yet.)*
- **Auto power-off / standby after idle** → Phase 4 **4D** (§7.5, task
  4D.19), feasibility-check-first per the task table.
- **Remember screen brightness / keypad backlight setting** → Phase 4
  **4D** (§7.5, task 4D.20), feasibility-check-first per the task table.
- **Desktop emulator build** (the tooling half of the old antialiasing
  item) → named as a candidate in Phase 6 §9 (see
  [phase6-spec.md](../phases/phase6-spec.md)), still unscoped/not
  committed — listed there rather than shipped.

## Completed / Closed

- **Fix tinyexpr's `(-2)^2 = -4`** (listed here 2026-08-09 after Phase 5.2's
  differential harness found it; **D50** scoped it out of that phase) →
  **shipped the same day as D51**, on `main`, released as **v0.3.2**,
  HW-verified on the Pico 2. It never spent a session on this list, which is
  the point: it was a bugfix that stood alone and was not gated on 5.2.
  **The estimate here was wrong in a useful direction** — "~5 lines,
  parse-time only" turned into a rewrite of `factor()`, because patching at
  the source exposed a **second** defect in the same function (`2^-3^2`
  returned 512: the right-associative insertion loop re-based a negated
  exponent). 5.2 covers neither that one nor graphing; only the vendored
  parser does. See D51 and `drivers/README.md` "Local modifications" — this
  is the project's first local fix to a vendored driver, so a re-vendor must
  re-apply it.

- **Coarsen too-dense grid lines** (usage feedback 2026-07-25) → **shipped
  same day**, no phase/D-number (small, localized fix). When `Xscl`/`Yscl`
  is tiny relative to the axis range, `GraphScreen::draw_axes` coarsens the
  grid step to the smallest multiple of `scl` spaced >= 4 px, so a
  wide/tall window draws the largest meaningful grid (~80 lines/axis max)
  instead of thousands of merged lines — faster and legible, no visual
  change at normal scales. Tick labels snap to the coarsened grid step so
  they stay on grid lines. HW-confirmed on the Pico 1. Shared `thin_factor`
  helper in `src/apps/graph_screen.cpp`.
- **Complex numbers** → Phase 4 sub-phase **4C**, shipped as **D30** (2026-07-20,
  see [decisions.md](./decisions.md) D30 and
  [phase4-spec.md](../phases/phase4-spec.md) §5). Adds a `Complex` type,
  `complexexpr` home-screen evaluator, `i`/`2i` syntax, complex-aware function
  set, and a MODE row entry. Matrix eigenvalues can now return a full complex
  spectrum as a formatted (unstorable) string via the new `Kind::kText` result.
  **Scope not fully met**: the original wishlist item committed to "lists and
  matrices hold complex values too," but D30 deferred that — `Variables::vars`
  and list storage (l1..l6) stay `calc_t`/real-only, so `2i->a` and storing a
  complex eigenvalue spectrum both error "Complex results can't be stored."
  **Partially graduated**: complex-valued *variable/Ans* storage → Phase 4
  **4D** (task 4D.15, see [phase4-spec.md](../phases/phase4-spec.md) §7.3).
  Complex-valued *lists and matrices* did **not** graduate — flagged in
  [design-departures-matrix-complex.md](design-departures-matrix-complex.md)
  as needing a Pico 1 memory feasibility study first, bigger than a 4D
  closing-pass item. Still unscoped.
- **TI-84 CALC-menu graph analysis** (value, zero, min/max, intersect, dy/dx,
  numeric fnInt) → Phase 4 sub-phase **4B**, shipped as **D29** (2026-07-20,
  see [decisions.md](./decisions.md) D29 and
  [phase4-spec.md](../phases/phase4-spec.md) §4). Numeric + interactive on the
  graph screen, layered on the existing compiled-eval machinery.
- **JuliaMono font swap (Session 13, D24.9)** → shipped **D31** as a general
  font selector: `-DPICOCALC_FONT=spleen|juliamono|iosevka|unifont|terminus`
  (default terminus). Licensing handled (OFL, vendored).
