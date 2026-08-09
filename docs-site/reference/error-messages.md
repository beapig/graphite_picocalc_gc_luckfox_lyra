# Error messages

What the calculator says when something goes wrong, and what to do about it.
Grouped by what you were probably doing.

## Parsing

| Message | Means |
|---|---|
| `Syntax error` | The expression could not be parsed |
| `Expression too long` | Beyond the input length limit |
| `Expression too complex` | Too many terms or nesting for the evaluator |
| `Too deeply nested` | Nested more levels deep than the parser allows |
| `Bad variable` | Not a valid variable name |

`Syntax error` is by far the most common, and the causes are predictable:

- **Case.** Everything is lowercase. `SIN(0)`, `Pi`, `L1` are all errors.
- **Wrong argument count.** Check the
  [Function catalog](function-catalog.md) — `round` takes two arguments, `seq`
  takes five.
- **Unbalanced parentheses.**
- **A bare `i`** in REAL mode. The imaginary unit is only bound in `a+bi` and
  `r∠θ` modes.

`Too deeply nested` has a fixed limit rather than a memory-dependent one.
Assigning part of the expression to a variable and referring to that is the
usual way around it.

## Numbers and mode

| Message | Means |
|---|---|
| `Non-real result` | The answer is not a real number, and Number mode is REAL |
| `Undefined result` | No defined value — a division by zero, say |
| `Undefined at point` | The function has no value at the requested point |
| `Undefined in interval` | The function is undefined somewhere in the interval |
| `Undefined slope` | The derivative does not exist there |

**`Non-real result` is a mode message, not a mistake.** `sqrt(-4)` is refused in
REAL mode and answers `2i` in `a+bi` mode. Switch on the MODE screen (`F3`),
Number row. See [Complex numbers](../guide/14-complex-numbers.md).

The `Undefined …` family comes from graph analysis. They mean the analysis
cannot proceed, usually because the interval you gave contains a point where the
function blows up. Narrow the bounds to exclude it.

## Storing

| Message | Means |
|---|---|
| `Bad store target` | The left side of `->` is not something you can store into |
| `Store target mismatch` | The value's type does not match the target |
| `e is reserved (Euler's e)` | `e` is Euler's number, not a variable |
| `i is reserved (imaginary unit)` | `i` is the imaginary unit |

Variables are lowercase `a`–`z` plus `theta`. `2->A` fails because input is
case-sensitive and `A` is not a variable.

## Lists

| Message | Means |
|---|---|
| `Expected a list` | A list argument got something else |
| `List length mismatch` | Two lists in one operation have different lengths |
| `List too long (max 10000)` | Beyond the per-list element limit |
| `Too many lists (20)` | Beyond the total list limit |
| `Too many named lists` | Beyond the named-list limit |
| `Out of list memory` | No room for another list of that size |
| `Non-real list` | A list holds complex values where reals were needed |
| `Too many list terms` | Too many terms in one list expression |

`List length mismatch` comes from element-wise arithmetic between lists of
different sizes. `length(l1)` and `length(l2)` will tell you which is which.

## Matrices

| Message | Means |
|---|---|
| `Expected a matrix` | A matrix argument got something else |
| `Dim mismatch` | The shapes do not allow that operation |
| `Singular matrix` | No inverse exists — the determinant is zero |
| `Matrix too large` | Beyond the size the calculator holds |
| `Matrix literal too large` | The typed literal exceeds that limit |
| `Expected (row, col)` | Element access needs two indices |
| `Matrix not allowed here` | A matrix appeared where a scalar was required |
| `Non-real matrix` | A matrix holds complex values where reals were needed |
| `Out of matrix memory` | No room for another matrix of that size |
| `Matrix division: use ^-1` | `[A]/[B]` is not defined — see below |

**On matrix division**: $AB^{-1}$ and $B^{-1}A$ are different matrices, so
"divide by a matrix" is ambiguous. Write `[A]*inverse([B])` or
`inverse([B])*[A]`, whichever you mean.

`Dim mismatch` on multiplication means the columns of the left do not match the
rows of the right. `dim([A])` reports the shape as a list.

## Sequences and solving

| Message | Means |
|---|---|
| `Bad seq range` | The sequence range is not valid |
| `Bad seq step` | The step is zero or the wrong sign for the range |
| `Syntax error in seq formula` | The formula inside `seq(...)` will not parse |
| `Bad solve bound` | A bound given to `solve` is not usable |
| `Bad bounds` | The interval is reversed or degenerate |

A step of the wrong sign is the usual cause: `seq(x,x,10,1,1)` counts up from 10
towards 1 and never arrives.

## Other

| Message | Means |
|---|---|
| `Unknown unit` | `convert` does not recognise that unit name |
| `Unknown mode` | An invalid mode setting |
| `Bad value` | The value is not valid in that field |
| `Too long` | Input exceeds a field's length |
| `Too complex` | The expression exceeds an internal limit |

## If the device reboots by itself

That is a crash, not an error message, and it is a bug worth reporting. The
**next** boot prints a line like:

```
fault: previous boot hard-faulted at pc=0x...
```

That address is the single most useful thing in a bug report. Please include
it, along with the board you are using, the firmware version, and the
expression that caused it — see the repository's issue templates.
