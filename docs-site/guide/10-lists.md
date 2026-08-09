# 10. Lists

A list is a sequence of numbers you can compute with as a unit. Lists are the
input to everything in [Statistics](11-statistics.md), and useful on their own
for applying a formula to many values at once.

## The six built-in lists

`l1` through `l6`, lowercase like everything else.

```
{1,2,3}->l1
```

Braces make a list literal. `->` stores it, exactly as with a scalar.

## Building lists without typing them

| Form | Gives |
|---|---|
| `range(1,9)` | 1, 2, …, 9 — step 1 |
| `range(0,1,.1)` | 0, 0.1, …, 1 — with an explicit step |
| `seq(x^2,x,1,10,1)` | $x^2$ for $x = 1 \ldots 10$ |

`seq` is the general one: an expression, the variable to vary, then low, high
and step.

```
seq(x^2,x,1,10,1)->l1
```

## Arithmetic is element-wise

Operations apply to every element:

```
l1+2*l2                    element-wise across two lists
{1,2,3}+2                  adds 2 to each element
```

A scalar combines with every element; two lists combine pairwise and must be
the same length, or you get `List length mismatch`.

Anywhere a list is expected you can use `l1`–`l6`, a named list, `range(...)`,
a `{...}` literal, or any expression that produces a list.

## Functions over lists

**Reducing to a single number:**

| Function | Gives |
|---|---|
| `sum(l)` | Total |
| `prod(l)` | Product |
| `length(l)` | Element count |
| `mean(l)` | Mean |
| `median(l)` | Median |
| `stdev(l)` | Sample standard deviation |

**Producing another list:**

| Function | Gives |
|---|---|
| `cumsum(l)` | Running totals |
| `delta_list(l)` | Differences between consecutive elements |
| `sort_asc(l)` | Sorted ascending |
| `sort_desc(l)` | Sorted descending |

Note that **`sort_asc` and `sort_desc` sort in place** — they reorder the
stored list rather than returning a sorted copy. If the original order matters,
copy the list first.

`cumsum` and `delta_list` are inverses of each other, which is occasionally
handy for converting between totals and increments.

## The list editor

Type **`lists`** (or `list`) on the home screen for a spreadsheet-style editor.

| Key | Action |
|-----|--------|
| Arrows | Move between cells |
| `ENTER` or just typing | Edit a cell, or append a new row |
| `DEL` | Delete a row |
| `F6` / `F7` (`Shift+F1`/`F2`) | Sort ascending / descending |
| `F8` (`Shift+F3`) | Clear the list |

`F8` clears immediately, without a confirmation step.

## Named lists

Lists are not limited to the six built-ins. Storing to a name creates one:

```
{1,2}->costs
```

Names are **2 to 5 characters and must start with a letter**. A named list
works anywhere `l1` works.

In the editor:

| Key | Action |
|-----|--------|
| `Alt+N` | New named list |
| `Alt+R` | Rename |
| `Alt+X` | Delete |

## Limits

| Limit | Value |
|---|---|
| Elements per list | 10,000 |
| Lists in total | 20 |

Exceeding these gives `List too long (max 10000)` or `Too many lists (20)`.
Long lists are held in the calculator's external memory rather than its main
RAM, so a 10,000-element list is genuinely usable rather than nominally
allowed.

## Lists and matrices

`list2mat(l1,l2)` builds a matrix whose columns are those lists;
`mat2list([A],l1)` goes the other way. See [Matrices](13-matrices.md).
