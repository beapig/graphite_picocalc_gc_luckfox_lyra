# 3. Functions and the catalog

Functions are called the ordinary way, with lowercase names:

```
sin(0)                     0
ln(e)                      1
ncr(5,2)                   10
```

## The catalog is the parser

The full list lives in two places that cannot disagree, because they are the
same table: the on-device help browser and the
[Function catalog](../reference/function-catalog.md) page on this site. Both
are generated from the table the expression parser registers from, so a
function that appears in either is a function the calculator accepts.

To browse it on the device, type **`help`** (or **`?`**) on the home screen.
It also carries the key reference and syntax notes.

## What is available

A summary by area — the catalog page has exact signatures and argument counts.

**Trigonometry** — `sin`, `cos`, `tan`, `asin`, `acos`, `atan`. These follow
the MODE screen's angle setting; `deg` and `rad` convert between the two
explicitly.

**Logarithms** — `log` (base 10) and `ln` (natural).

**Combinatorics** — `fac` (factorial), `ncr` (n choose r), `npr`
(permutations).

**Numeric helpers** — `round`, `min`, `max`, `rand`.

**Lists** — `sum`, `prod`, `length`, `mean`, `median`, `stdev`, `sort_asc`,
`sort_desc`, `cumsum`, `delta_list`, `seq`, `range`. See [Lists](10-lists.md).

**Distributions** — `normal_*`, `t_*`, `chisq_*`, `f_*`, `binomial_*`,
`poisson_*`, `geometric_*`, each with density/mass, cumulative and (for the
continuous ones) inverse variants. See
[Distributions and inference](12-distributions-and-inference.md).

**Matrices** — `det`, `inverse`, `transpose`, `rref`, `ref`, `rank`,
`identity`, `augment`, `dim`, `eigenvals`, `eig`, `eigenvec`. See
[Matrices](13-matrices.md).

**Vectors** — `dot`, `cross`, `norm`.

**Solving** — `solve(f, x, lo, hi)` finds a root numerically.

**Conversion** — `convert(v, from, to)` for units, and `list2mat` / `mat2list`
between lists and matrices.

## Factorial has two spellings

`n!` and `fac(n)` mean the same thing. The postfix form is rewritten to the
function form before parsing, so both are equally valid anywhere.

```
5!                         120
fac(5)                     120
```

## Angle mode applies to trig

`sin(30)` is $0.5$ in DEGREE mode and $-0.988\ldots$ in RADIAN mode. Neither is
wrong — check the MODE screen (`F3`) if a trig answer surprises you. It is the
single most common cause of an unexpected result.

The inverse functions return an angle in whichever mode is active, so
`asin(0.5)` is $30$ in DEGREE and $0.5236$ in RADIAN.

## CAS functions

`simplify`, `expand`, `factor`, `diff`, `integ` and `solve` are callable inline
like any other function:

```
diff(x^2, x)               2x
```

They also have a menu, on `F6` from the home screen, or the typed command
`cas`. See the symbolic-math section of the main feature list.

## When a call does not work

Common causes, roughly in order of likelihood:

- **Case.** Everything is lowercase. `Sin(0)` is a syntax error.
- **Wrong number of arguments.** The catalog page lists the arity of every
  function; `round` takes two, `seq` takes five.
- **Angle mode**, as above.
- **Number mode.** In REAL mode, an expression whose answer is not real
  reports `Non-real result` rather than returning `NaN`. See
  [Complex numbers](14-complex-numbers.md).
