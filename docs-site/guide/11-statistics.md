# 11. Statistics

Statistics work on [lists](10-lists.md). Get your data into `l1`–`l6` or a
named list first, then type **`stats`** (or `stat`) on the home screen.

## The stats screen

| Key | Action |
|-----|--------|
| `UP` / `DOWN` | Select a row |
| `LEFT` / `RIGHT` | Change the value on that row |
| `ENTER` on the last row | Calculate |
| `UP` / `DOWN` in results | Scroll the results |

You pick the calculation and which lists it runs over, then calculate. Results
are often longer than the screen — scroll them.

## 1-Var and 2-Var

**1-Var** describes a single list: mean, standard deviations, the five-number
summary, and the sums behind them. It accepts an optional **frequency list**, so
data already tallied into counts does not have to be expanded first — put the
values in one list and their counts in another.

**2-Var** describes a pair of lists together: the per-variable statistics plus
the joint sums that regression rests on.

## Regression

Ten models, the TI-83/84 set:

| | |
|---|---|
| Linear | Quadratic |
| Cubic | Quartic |
| Logarithmic | Exponential |
| Power | Logistic |
| Sinusoidal | Median-Median |

Each fits your x and y lists and reports the coefficients.

**Store to** is the part worth knowing: it writes the fitted model into a Y=
slot as an expression. The fit then plots alongside your data like any other
function, and traces, and appears in the table. That is the quickest way to see
whether a fit is any good — a correlation coefficient tells you less than
looking at the curve against the points.

Choosing a model is your judgement, not the calculator's. Median-Median is the
one to reach for when outliers are distorting a linear fit, since it is
resistant to them in a way least-squares is not.

## Stat plots

Type **`plot`** (or `plots`). There are **three slots**, each of which can be:

| Plot | Shows |
|---|---|
| Scatter | Points from an x list against a y list |
| xy-Line | The same points joined in list order |
| Histogram | Frequency of values in bins |
| Box | Five-number summary as a box-and-whisker |
| Normal probability | Data against normal quantiles, to judge normality |

Stat plots draw **on the graph screen, alongside your functions**. That is
deliberate: a regression stored to a Y= slot and its scatter plot appear
together without any extra step.

### Fitting the window to data

Press **`Z`** on the graph for **ZoomStat**, which sizes the window to the
active stat plots rather than to your functions. Data rarely lives inside the
default $\pm 10$ window, so a scatter plot that appears blank usually just needs
`Z`.

## A usual sequence

1. `lists` — type the data into `l1` and `l2`.
2. `plot` — turn on a scatter of `l1` against `l2`.
3. `F5`, then `Z` — graph, and fit the window to the data.
4. `stats` — run a linear regression, **Store to** `Y1`.
5. `F5` — the fitted line is now drawn over the points.
6. If it fits badly, go back to step 4 and try another model.

## Related

- Probability distributions and hypothesis tests:
  [Distributions and inference](12-distributions-and-inference.md)
- The underlying list operations: [Lists](10-lists.md)
- Function signatures: [Function catalog](../reference/function-catalog.md)
