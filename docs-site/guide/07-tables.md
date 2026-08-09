# 7. Tables

A table lists your functions' values at regular steps — the numeric view of the
same expressions the graph draws.

Open it with **`F5`** from the graph screen.

## Keys

| Key | Action |
|-----|--------|
| `UP` / `DOWN` | Scroll rows |
| `LEFT` / `RIGHT` | Scroll columns |
| `F2` SETP | Table setup |
| `ENTER` | Add a value (ASK mode only) |
| `DEL` | Delete a row (ASK mode only) |
| `Alt+F5` | Split screen, graph and table together |

## Columns follow the graph mode

| Mode | Columns |
|---|---|
| Function | `x`, then `Y1`, `Y2`, … |
| Parametric | `T`, then `X1T`, `Y1T`, … |
| Polar | `th`, then `r1`, `r2`, … |

With several slots enabled there are more columns than fit on screen; `LEFT`
and `RIGHT` scroll them sideways. The independent-variable column stays put so
you always know which row you are reading.

## AUTO and ASK

Set by **`F2`** (table setup), which also sets the start value and the step.

**AUTO** fills the table itself, from your start value in steps of your step
size. It scrolls **infinitely in both directions** — hold `DOWN` and it keeps
generating rows, and it goes backwards past the start value too. There is no
end of the table to reach.

**ASK** starts empty and shows only values you type. Press `ENTER` to add one,
`DEL` to remove a row. Use this when you care about specific inputs rather than
a regular sweep — checking a handful of awkward values, say.

## Table setup

`F2` from the table:

| Field | Meaning |
|---|---|
| Start | The first independent-variable value |
| Step | The increment between rows (AUTO only) |
| AUTO / ASK | Which of the two modes above |

A step of 0.1 with a start of 0 is a common choice for looking at behaviour
near the origin; a step of 1 suits integer-valued problems.

## Reading a table against a graph

The split screen (`Alt+F5`) shows both at once with the trace cursor and the
selected table row kept in sync — moving one moves the other. That is often the
fastest way to connect a feature you can see to the numbers behind it. See
[Split screen](09-split-screen.md).

## Undefined values

Where a function has no value — `1/x` at zero, `sqrt(x)` for negative `x` in
REAL mode — the cell shows that rather than a number. A gap in the table
column corresponds to a gap in the plotted curve.
