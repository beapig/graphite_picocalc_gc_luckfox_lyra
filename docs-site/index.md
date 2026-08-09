# Graphite — PicoCalc graphing calculator

A TI-83/84-inspired graphing calculator, running as firmware on the
[ClockworkPi PicoCalc](https://www.clockworkpi.com/picocalc) handheld — built
from a Raspberry Pi Pico or Pico 2 module, and open source.

This site is the **user guide**: how to build and flash it, how to use it, and
reference material generated straight from the firmware source so it cannot
drift out of date.

## What it does

- **Calculating** — natural math display with stacked fractions and raised
  exponents, and exact answers where they exist: `sqrt(8)` shows as $2\sqrt{2}$,
  `sin(pi/3)` as $\sqrt{3}/2$.
- **Graphing** — function, parametric, polar and sequence modes, with trace,
  a full zoom family, shading, value tables and a split graph-and-table view.
- **Analysis** — zeros, extrema, intersections, numeric derivatives and
  integrals, cursor-driven on the curve.
- **Statistics** — six lists plus named lists, 1-var and 2-var statistics, ten
  regression models, seven distributions, hypothesis tests, confidence
  intervals, ANOVA, and five kinds of stat plot.
- **Linear algebra** — ten matrix variables with determinant, inverse, row
  reduction, rank, eigenvalues and eigenvectors.
- **Complex numbers** — throughout, including complex-valued lists and
  matrices.
- **Symbolic math** — simplify, expand, factor, differentiate, solve, and a
  bounded symbolic integrator.

Everything persists to the SD card, so the calculator is where you left it after
a power cycle.

## Where to start

| | |
|---|---|
| Not flashed yet | [Build and flash](getting-started/build-and-flash.md) |
| Just flashed it | [First steps](getting-started/first-steps.md) |
| Looking for a function | [Function catalog](reference/function-catalog.md) |
| Looking for a key | [Key reference](reference/key-reference.md) |
| Syntax question | [Syntax reference](reference/syntax-reference.md) |

The guide chapters work through each area in turn, starting with
[Basic calculations](guide/01-basic-calculations.md).

## On the device itself

Type **`help`** (or `?`) on the home screen. The built-in browser carries the
function catalog, a per-screen key reference and syntax notes — and its catalog
is the same table the expression parser registers from, so it always describes
the firmware you are actually running.

The three reference pages on this site are generated from that same source.

## Project status

The calculator is feature-complete as a graphing calculator and has a working
CAS. Programming — an app framework with MicroPython — is planned and not yet
started, which is why [chapter 15](guide/15-programming.md) is a placeholder.

## What this is not

This is the user guide. Developer material — phase contracts, the decision log,
architecture and hardware bring-up notes — lives in the repository's `docs/`
tree and is not republished here.

One exception, because it is written to be read rather than referred to:
[How it works](internals/index.md), a study guide to the algorithms behind the
calculator. Its first chapter,
[What went wrong](internals/13-what-went-wrong.md), walks through four real
bugs from symptom to cause.
