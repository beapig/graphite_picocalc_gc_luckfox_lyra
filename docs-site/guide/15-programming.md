# 15. Programming

**Not yet implemented.**

Programmability is planned as Phase 6 of the project, in two parts:

- **An app framework** — a way to run things that are not the calculator, with
  a defined lifecycle and screen contract.
- **MicroPython** as the first app on that framework, exposing a `calc` module
  so scripts can reach the calculator's math engine: evaluating expressions,
  reading and writing variables, lists and matrices from Python.

Neither has started as of this writing. This chapter holds its place in the
guide so the numbering stays stable when it does.

## What exists today instead

The calculator is not programmable, but several things cover ground that a
program otherwise would:

- **`seq`** applies an expression across a range and collects the results into
  a list, which handles most "do this for every value of $x$" tasks:

  ```
  seq(x^2,x,1,10,1)->l1
  ```

- **Element-wise list arithmetic** applies a formula to a whole dataset at once
  — see [Lists](10-lists.md).
- **Sequence graph mode** evaluates recurrences, including ones that refer to
  their own earlier terms, which covers iterative calculations. See
  [Parametric, polar and sequence](08-parametric-polar-sequence.md).
- **Regression Store-to** writes a fitted model into a Y= slot as a working
  expression — see [Statistics](11-statistics.md).
- **The CAS** does symbolic manipulation directly, so some things you would
  otherwise write a program for do not need one.

## Following along

Progress is tracked in the repository's roadmap and issue list. When Phase 6
ships, this chapter will be written and an app-developer guide will cover
writing MicroPython apps in more depth than a user chapter should.
