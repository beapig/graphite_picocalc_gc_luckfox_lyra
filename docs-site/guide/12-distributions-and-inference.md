# 12. Distributions and inference

Two related tools: **`dist`** computes probabilities from a distribution, and
**`test`** runs hypothesis tests and confidence intervals.

Both are also available as ordinary functions you can type into an expression —
the screens are a convenience, not a separate system.

## Distributions

Type **`dist`**.

| Key | Action |
|-----|--------|
| `LEFT` / `RIGHT` | Choose the distribution, and the function |
| `ENTER` | Edit a parameter field |
| `DEL` | Clear a field and start an empty edit |
| `ENTER` on the last row | Calculate |

The result goes into `ans`, **and the equivalent function call is shown**. That
second part is worth noticing: once you have used the screen a few times you can
type the call directly and skip the screen entirely.

### What is available

**Continuous** — normal, $t$, $\chi^2$, $F$. Each has three forms:

| Suffix | Gives |
|---|---|
| `_pdf` | Density at a point |
| `_cdf(lo, hi, …)` | $P(lo \le X \le hi)$ |
| `_inv` | Inverse CDF, from a lower-tail area |

**Discrete** — binomial, Poisson, geometric. These have `_pmf` (probability of
exactly $k$) and `_cdf` (probability of at most $k$). Their $k$ and $n$
arguments **must be integers**.

### Two things that catch people out

**The CDF takes two bounds, not one.** `normal_cdf(lo, hi, mu, sd)` is
$P(lo \le X \le hi)$ — an interval, not a lower tail. For an open tail, pass a
huge bound: use `-1e99` for "no lower limit" and `1e99` for "no upper limit".

```
normal_cdf(-1e99, 1.96, 0, 1)      lower tail below 1.96
normal_cdf(1.96, 1e99, 0, 1)       upper tail above 1.96
```

**The inverse takes a lower-tail area.** `normal_inv(0.975, 0, 1)` gives the
value with 97.5% below it. If you are thinking in terms of an upper tail,
subtract from 1 first.

## Inference

Type **`test`** (or `infer`).

| Key | Action |
|-----|--------|
| `LEFT` / `RIGHT` | Cycle the test, and its options |
| `ENTER` | Edit a field, or run the test |

The suite covers hypothesis tests, confidence intervals, and ANOVA.

### Data or Stats

Where it applies, a test offers two input sources:

- **Data** — point it at a list, and it computes the summary statistics itself.
- **Stats** — type the summary statistics directly (mean, standard deviation,
  sample size) when you have those but not the raw data.

Choose Data when you have the observations; it is fewer numbers to type and
fewer chances to mistype one.

## Using them from an expression

Everything above is a function. For a one-off, typing it is faster than opening
a screen:

```
normal_cdf(-1e99, 1.96, 0, 1)      0.9750021049
binomial_pmf(3, 10, 0.5)           0.1171875
t_inv(0.975, 20)                   2.085963447
```

Signatures for all of them are in the
[Function catalog](../reference/function-catalog.md), which is generated from
the same table the parser uses.

## Related

- Getting data into lists: [Lists](10-lists.md)
- Descriptive statistics and regression: [Statistics](11-statistics.md)
