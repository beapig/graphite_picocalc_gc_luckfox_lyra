# 4. Graphing

This chapter covers function mode — plotting $y = f(x)$. Parametric, polar and
sequence modes are in
[Parametric, polar and sequence](08-parametric-polar-sequence.md), and they
share everything here except the editor and the independent variable.

## The Y= editor

Press **`F1`** from anywhere to open the editor for the active graph mode. In
function mode that is the Y= editor, with **seven slots**, `Y1` through `Y7`.

| Key | Action |
|-----|--------|
| `UP` / `DOWN` | Select a slot |
| `ENTER` | Edit the selected slot |
| `SPACE` | Toggle the slot on or off |
| `DEL` | Clear the slot |
| `F5` | Graph |

Type an expression in terms of `x`. A slot that will not parse is drawn in
**red**, so you can see a typo without leaving the editor.

Toggling a slot off keeps the expression but stops plotting it — useful for
comparing curves without retyping them.

## Drawing the graph

**`F5`** graphs. Each enabled slot is drawn in its own colour, which is also
the colour used for its trace cursor and for any shading it produces, so a
plot with several curves stays readable.

The axes and a grid are drawn behind the curves. **`L`** toggles the axis
labels if they get in the way of what you are looking at.

## Discontinuities

A function like `1/x` or `tan(x)` jumps between samples. The plotter detects
these and breaks the curve rather than drawing a near-vertical line across the
screen joining $+\infty$ to $-\infty$.

That vertical line is a well-known artifact on graphing calculators; if you are
used to seeing it, its absence here is deliberate.

Points where the function is undefined are simply not plotted, which is also
why `sqrt(x)` in REAL mode draws nothing left of the origin rather than
drawing something wrong.

## Shading

Two kinds:

- **`H`** on the graph screen shades between a lower and an upper expression.
- **`S`** in the Y= editor sets a per-slot shade style — above or below that
  curve.

Shading follows the curve's own colour, darkened, so it stays distinguishable
when several are on screen.

## Where to go from the graph

| Key | Goes to |
|-----|---------|
| `F4` | Trace — walk along a curve, reading coordinates |
| `F6` | CALC — zeros, extrema, intersections, derivative, integral |
| `F5` | Value table |
| `Alt+F5` | Split screen, graph above and table below |
| `F1` | Back to the editor |
| `F2` | Window settings |

Tracing and CALC are in [Tracing and the CALC menu](06-tracing-and-calc-menu.md).
Setting the visible region is in [Window and zoom](05-window-and-zoom.md).

## A note on `x`

Graphing evaluates your expression once per sample point with `x` set to that
point, so **whatever you had stored in `x` is overwritten by drawing a graph**.
That is normal and matches how TI calculators behave, but it does mean `x` is a
poor choice for storing a value you want to keep.
