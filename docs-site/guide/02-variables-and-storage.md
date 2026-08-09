# 2. Variables and storage

## Storing a value

The store operator is `->`:

```
2->a                       2
a*10                       20
```

The stored value is also the result of the store, which is why `2->a` displays
`2`.

## Which names you can use

Variables are the **lowercase** letters `a` through `z`, plus `theta`. Input is
case-sensitive, so `2->A` is a syntax error, not a store into `a`.

Two letters are reserved and cannot be stored into:

| Name | Why |
|---|---|
| `e` | Euler's number, $2.718\ldots$ |
| `i` | The imaginary unit |

Storing to either is refused with a message saying so — `e is reserved
(Euler's e)` — rather than failing silently. That leaves 24 free letters plus
`theta`.

`theta` is spelled out in full. It exists because polar graphing uses it as the
independent variable, but you can store to it like any other variable.

## `ans`

`ans` always holds the most recent result. It updates on every evaluation, so
it is a convenience rather than storage — if you want to keep a value, put it
in a letter.

```
5*4                        20
ans+2                      22
ans->b                     22
```

## `x` is not special, except when graphing

`x` is an ordinary variable on the home screen. When the calculator plots
`Y1`, it evaluates that expression with `x` set to each sample point, so
whatever you had stored in `x` is overwritten as a side effect of graphing.

Analysis on the graph screen also writes back: finding a root with CALC leaves
the answer in both `x` and `ans`.

If you are keeping a value you care about, `x` is the one letter to avoid.

## Persistence

Variables are written to the SD card and survive a power cycle, along with
history, graph state, lists, matrices and your display settings. There is no
"save" step — the calculator does it for you.

This also means a variable you set weeks ago is still there. If a formula gives
an unexpected answer, a stale variable is worth checking before anything else.

## Lists and matrices are stored differently

`l1` through `l6`, named lists, and the matrices `[A]` through `[J]` are not
letter variables and do not compete with them for names. They have their own
storage and their own editors:

```
{1,2,3}->l1
{1,2}->costs
```

See [Lists](10-lists.md) and [Matrices](13-matrices.md).

## Constants

`pi` and `e` are built in. Scientific and unit constants are available through
`convert` and the constants catalog — see
[Functions and the catalog](03-functions-and-catalog.md).

Remember that `e` alone is Euler's number, but `e` inside a numeric literal is
exponent notation: `1e10` is $10^{10}$, a single number, not `1 * e * 10`.
