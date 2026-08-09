# 5. Window and zoom

The window is the region of the plane you are looking at. Everything in this
chapter changes only that — your functions are untouched.

## The WINDOW screen

**`F2`** from anywhere.

| Key | Action |
|-----|--------|
| `UP` / `DOWN` | Select a field |
| `ENTER` | Edit the value |
| `DEL` | Clear it and start an empty edit |

The fields are mode-aware. Function mode shows the shared x and y bounds;
parametric mode adds `Tmin` / `Tmax` / `Tstep` above them, and polar mode adds
`THmin` / `THmax` / `THstep`. The x/y bounds are shared across all modes, so
setting them once carries over when you switch.

## Zoom keys

All of these are single letters pressed on the **graph** screen.

| Key | Does |
|-----|------|
| `=` | Zoom in |
| `-` | Zoom out |
| `S` | **ZStandard** — the default window |
| `T` | **ZTrig** — sized for trig functions |
| `D` | **ZDecimal** — pixel steps land on tidy decimal values |
| `Q` | **ZSquare** — equal scale on both axes |
| `B` | **ZBox** — pick two corners, zoom to that rectangle |
| `F` | **ZoomFit** — keep x, fit y to the curves |
| `Z` | **ZoomStat** — fit the window to your stat plots |

Two of these are worth knowing well:

**ZoomFit (`F`)** is the one to reach for when a curve is off-screen and you do
not want to work out the y range by hand. It keeps your x bounds and picks y to
contain what is actually plotted.

**ZBox (`B`)** is the fastest way to inspect a feature. Press it, move to one
corner, `ENTER`, move to the opposite corner, `ENTER`.

## Why ZStandard is not $\pm 10$ on both axes

ZStandard sets $x = \pm 10$, but $y = \pm 8.75$ rather than $\pm 10$.

That is deliberate. The plot area is 320 by 280 pixels, so equal numeric ranges
on both axes would be *displayed* squashed — a circle would come out about
12.5% wider than tall. Deriving y from the aspect ratio
($10 \times 280/320 = 8.75$) makes the default window square **as you see it**,
which is what people actually want from a standard window.

If you want equal scaling from some other window, `Q` (ZSquare) does the same
correction to whatever you currently have.

## Stat plots

`Z` (ZoomStat) sizes the window to your active stat plots rather than to your
functions. It is the counterpart of ZoomFit for data. See
[Statistics](11-statistics.md).

## Persistence

The window is saved with the rest of the graph state, so it is still there next
time you power on. If a graph looks blank after a break, `S` (ZStandard) is the
quickest way to get back to somewhere sensible.
