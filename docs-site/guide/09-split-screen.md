# 9. Split screen

Split screen shows the graph above and the value table below, with the two kept
in sync.

Open it with **`Alt+F5`** from either the graph or the table.

| Key | Action |
|-----|--------|
| `F5` | Switch which pane has focus |
| `F4` | Trace, in the graph pane |
| `Alt+F5` or `ESC` | Back to full screen |

## The sync is the point

Trace position and table row track each other. Move the trace cursor and the
table scrolls to the matching row; scroll to a table row and the cursor moves
to that point on the curve.

That connection is what the split view is for. A feature you can *see* — a
turning point, a zero crossing, somewhere the curve does something abrupt —
gets its exact numbers next to it without switching screens and losing your
place.

## Working in it

Only one pane takes arrow keys at a time; `F5` moves focus between them. The
focused pane is indicated on screen.

- **Graph pane focused**: arrows move the trace cursor, and the table follows.
- **Table pane focused**: arrows scroll rows and columns, and the cursor
  follows.

Both panes are smaller than the full-screen versions, so the graph shows less
detail and the table fewer rows. For close work on one or the other, go back to
full screen — the split is for relating them, not for replacing either.

## A worked pattern

Finding where a function crosses a particular value:

1. Graph it and open the split (`Alt+F5`).
2. Trace (`F4`) to roughly where the crossing looks like it is.
3. Read the table rows either side of the cursor for the exact bracketing
   values.
4. If you want the crossing precisely rather than bracketed, use CALC's **zero**
   on the full-screen graph — see
   [Tracing and the CALC menu](06-tracing-and-calc-menu.md).

Step 4 is worth remembering. The split screen is good at showing you *where* to
look; CALC is what gives you the answer.
