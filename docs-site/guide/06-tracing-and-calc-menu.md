# 6. Tracing and the CALC menu

## Trace

**`F4`** toggles trace. It works from any screen — if you are not on the graph,
`F4` takes you there and starts tracing.

| Key | Action |
|-----|--------|
| `LEFT` / `RIGHT` | Move the cursor along the curve |
| `UP` / `DOWN` | Switch to the next curve |
| `F4` | Leave trace |

A cursor sits on the curve with a vertical line marking its position, drawn in
that slot's colour, and the coordinates appear as a readout.

The readout adapts to the graph mode:

| Mode | Shows |
|---|---|
| Function | `x` and `y` |
| Parametric | `t`, with the x/y it maps to |
| Polar | `θ`, with the r it maps to |

The cursor follows the curve rather than the pixel grid, so the values you read
are the function's, not an approximation from where a pixel happened to land.

## The CALC menu

**`F6`** on the graph screen, or the typed command `calc`, opens graph
analysis. Everything here is numeric and cursor-driven — you point at roughly
where you want an answer and the calculator finds it precisely.

| Item | Finds |
|---|---|
| **value** | $y$ at an $x$ you type |
| **zero** | A root, between two bounds you place |
| **minimum** | A local minimum, within bounds |
| **maximum** | A local maximum, within bounds |
| **intersect** | Where two curves cross |
| **dy/dx** | The numeric derivative at a point |
| **$\int f(x)\,dx$** | The numeric integral between two bounds |

`ENTER` places each bound or point; `ESC` cancels.

All of it works across function, parametric and polar modes.

## How bounded operations work

Zero, minimum, maximum and the integral all ask for a **left bound** and a
**right bound**, then work inside that interval:

1. Move the cursor left of the feature, `ENTER`.
2. Move right of it, `ENTER`.
3. The answer appears.

This is why they are reliable on functions with several roots — you are telling
the calculator which one you mean. If the interval contains no root, or the
function is undefined somewhere inside it, you get a message saying so
(`Undefined in interval`) rather than a wrong number.

**intersect** works the same way but asks you to pick the two curves first.

## Results go into variables

A root is written back to both **`x`** and **`ans`**, so you can use it
immediately in the next calculation without copying digits off the screen.

That also means finding a root overwrites whatever was in `x` — see
[Variables and storage](02-variables-and-storage.md).

## Trace versus CALC

They answer different questions and it is worth being clear which you want:

- **Trace** reads off values *you navigate to*. Good for exploring the shape of
  a curve.
- **CALC** finds features *precisely*, wherever they are in the interval you
  give. Good for answers.

Tracing to what looks like a minimum gives you the nearest sample point. CALC's
minimum gives you the minimum.
