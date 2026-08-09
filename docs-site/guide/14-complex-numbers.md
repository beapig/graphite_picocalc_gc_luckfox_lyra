# 14. Complex numbers

## The Number mode decides everything

The MODE screen (**`F3`**) has a *Number* row with three settings:

| Mode | Behaviour |
|---|---|
| **REAL** | Only real results. A non-real answer reports `Non-real result` |
| **`a+bi`** | Rectangular — results shown as $a + bi$ |
| **`r∠θ`** | Polar — results shown as a magnitude and an angle |

This is the first thing to check when a complex calculation does not do what you
expect. It is not a display preference alone: in REAL mode, an expression whose
answer is not real is refused rather than approximated.

```
sqrt(-4)      REAL     Non-real result
sqrt(-4)      a+bi     2i
```

Reporting `Non-real result` rather than returning `NaN` is deliberate — it tells
you the mode is the problem, not your expression.

## Entering complex numbers

`i` is the imaginary unit, and is reserved: you cannot store to it.

```
3+2i
sqrt(-4)
(2+3i)*(1-i)
```

In `r∠θ` mode the angle follows the MODE screen's angle setting, like every
other angle in the calculator, so the same number displays differently in
RADIAN and DEGREE.

## What works with complex values

Arithmetic and the elementary functions are all complex-aware. So are the
containers:

- **Variables** — a letter can hold a complex value.
- **Lists** — a list can contain complex elements.
- **Matrices** — including the linear algebra. `eigenvals` of a rotation matrix
  gives a complex spectrum rather than an error.

That last point is the practical reason to switch to `a+bi` even if you are not
doing complex arithmetic deliberately: real matrices routinely have complex
eigenvalues.

## Integer powers are exact

`(2+3i)^2` is computed directly rather than as $e^{2\ln(2+3i)}$. The general
route accumulates a small error that can show up in the last digit or two of an
otherwise exact answer, so integer powers take a path that does not.

You are unlikely to notice this working — which is the point. It is mentioned
because it is the kind of thing that produces a puzzling final digit if it is
*not* done.

## Trig in DEGREE mode

Complex trig functions honour the angle mode, and scale the whole argument.
The consequence worth knowing: **for a real-valued input, the complex and real
evaluators give the same answer in either mode**. Switching to `a+bi` never
silently changes a real calculation.

`sin(2+3i)` in DEGREE mode means $\sin\big((2+3i) \times \pi/180\big)$. No
calculator agrees with every other on this input; this is the interpretation
that degrades correctly to the real case.

## Related

- Which functions accept what: [Function catalog](../reference/function-catalog.md)
- Complex matrices: [Matrices](13-matrices.md)
- Exact-form display: [Basic calculations](01-basic-calculations.md)
