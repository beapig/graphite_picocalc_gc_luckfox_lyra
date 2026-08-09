# 8. Parametric, polar, and sequence modes

Function mode plots $y = f(x)$. Three other modes plot other things, chosen on
the MODE screen (**`F3`**) under *Graph mode*.

Switching mode changes which editor `F1` opens, which fields the WINDOW screen
shows, and which columns the table has. Your function-mode expressions are kept
— switching back brings them straight up.

## Parametric

Plots $x$ and $y$ as separate functions of a parameter $t$:

$$x = X_{1T}(t), \qquad y = Y_{1T}(t)$$

**Six pairs**, `X1T`/`Y1T` through `X6T`/`Y6T`. In the editor, committing an X
expression jumps straight to its empty Y partner, since a pair is only useful
complete.

The parameter runs from `Tmin` to `Tmax` in steps of `Tstep`, all set on the
WINDOW screen (`F2`). These matter more than window bounds usually do — too
large a `Tstep` and a smooth curve comes out as a polygon; too small and
drawing is slow.

A circle of radius 3:

```
X1T = 3*cos(t)
Y1T = 3*sin(t)
```

with `Tmin` 0, `Tmax` $2\pi$ in RADIAN mode (or 360 in DEGREE). Use `Q`
(ZSquare) on the graph if it comes out elliptical — that is the window's aspect,
not your maths.

## Polar

Plots $r$ as a function of $\theta$: **six slots**, `r1` through `r6`.

The angle runs from `THmin` to `THmax` in steps of `THstep`, on the WINDOW
screen. **The MODE screen's angle setting applies to $\theta$**, so a full turn
is $2\pi$ in RADIAN mode and 360 in DEGREE mode — getting this wrong is the
usual reason a polar plot comes out as a fragment of what you expected.

Type the variable as `theta`, spelled out:

```
r1 = 2*cos(3*theta)
```

## Sequence

Plots sequences defined by recurrence: **three slots**, `u`, `v` and `w`.

Each is written in terms of `n`, and may refer to its own earlier terms —
**lags of one and two** are supported, `u(n-1)` and `u(n-2)`:

```
u(n) = u(n-1) + 1
```

A recurrence needs a starting value. `u(nMin)` seeds the first term, and a
second seed is available for sequences that reach back two terms.

Sequences may also refer to each other, so `v` can be defined in terms of `u`.

### TIME and WEB

MODE has a *Seq plot* row with two styles:

- **TIME** plots the terms against $n$ — the ordinary view of a sequence.
- **WEB** draws a cobweb diagram, stepping between the curve and the line
  $y = x$. This is the view that shows convergence: a cobweb spiralling inward
  means the sequence settles, spiralling outward means it diverges.

## What carries across modes

| Shared | Per-mode |
|---|---|
| The x/y window bounds | `Tmin`/`Tmax`/`Tstep`, `THmin`/`THmax`/`THstep` |
| Zoom keys | Which editor `F1` opens |
| Trace, CALC, table, split screen | The table's columns and trace readout |

Trace reports the parameter as well as the point — `t` in parametric, `θ` in
polar — because on these curves the position alone does not tell you where you
are in the traversal.
