# How it works

A study guide to the algorithms this calculator is built from — written to be
readable by someone who will never open the source, and useful to someone who
will.

Each chapter has the same shape: **the algorithm itself**, taught on its own
terms and independently of this codebase; then **how it is implemented here**,
with the constraint that shaped the implementation; then **what it cost**, as a
number taken from a measurement rather than an impression.

The constraints are the interesting part. Most compiler and numerics tutorials
are written against unbounded memory. This one runs on a microcontroller with
264 KB of RAM, a **4 KB call stack shared with a display service on the other
core**, no floating-point unit on one of its two supported chips, and 8 MB of
extra memory that is not memory-mapped and has to be reached over SPI. Those
limits decided most of the design, and they are stated wherever they did.

## Chapters

Written:

- [13. What went wrong](13-what-went-wrong.md) — four real bugs, start to
  finish, including the wrong explanations

Planned, in this order:

| | Chapter | The algorithm |
|---|---|---|
| 01 | Tokenizer | Lexing infix expressions; implicit multiplication; the `-` ambiguity |
| 02 | Shunting yard | Dijkstra's algorithm — precedence, associativity, unary operators, function calls |
| 03 | RPN stack machine | Compile once, evaluate many; a flat program versus a tree walk |
| 04 | Tagged values | One 24-byte value over reals, complex numbers, matrices and lists, without RTTI or virtuals |
| 05 | Natural math rendering | Layout trees, box metrics and baselines — stacked fractions and raised exponents |
| 06 | Graphing pipeline | Viewport transforms, sampling, discontinuity detection, trace |
| 07 | Numeric algorithms | Root finding, numeric integration, the distribution functions |
| 08 | Linear algebra | LU decomposition, determinant, inverse, row echelon, eigenvalues |
| 09 | CAS passes | Simplify, expand, factor and differentiate as tree rewrites over a pool |
| 10 | Symbolic integration | Pattern matching, and where the Risch algorithm begins |
| 11 | Memory on a microcontroller | Static allocation, arenas, pools, PSRAM over SPI, DMA |
| 12 | Two cores | A display service on the second core, and what that does to everyone else's stack |

## Why chapter 13 is written first

Because it is the only one that could not have been written by reading the code.

The algorithm chapters describe designs that can be reconstructed from the source
by anyone patient enough. Chapter 13 describes what happened when those designs
met real hardware — which board faulted, which test was the wrong instrument,
which hypothesis was good and wrong. That information exists only because it was
written down as it was discovered, and it decays fast.

It is also the honest answer to "what is it actually like to build this?", which
is the question a study guide should answer and a reference manual cannot.
