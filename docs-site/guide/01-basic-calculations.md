# 1. Basic calculations

The home screen is where the calculator starts and where it returns when you
press `HOME`. Type an expression, press `ENTER`, and the result appears on the
right of the line below.

```
2+3*4                      14
(2+3)*4                    20
```

Multiplication and division bind tighter than addition and subtraction, and
`^` binds tighter still, so `2+3*4` is 14 rather than 20. `^` is
right-associative, which is the usual mathematical convention: `2^3^2` is
$2^{(3^2)} = 512$, not $(2^3)^2 = 64$.

Unary minus binds to its own operand. `-2^2` is $-(2^2) = -4$, but `(-2)^2` is
$4$ — the parentheses make the negation part of the base.

## Case matters

**Input is case-sensitive, and everything is lowercase**: function names,
variable names and commands. `sin(0)` works; `SIN(0)` is a syntax error.

## Natural math display

Results and input are drawn the way they would be written rather than as a flat
line of text. Fractions stack, exponents are raised, and parentheses grow to
match what they enclose.

That is a display convention, not a different input syntax — you still type
`1/3` and `x^2` in the ordinary way.

## Exact forms

When a result has a clean closed form, the calculator shows **that** instead of
a decimal, in amber:

| You type | You get |
|---|---|
| `sqrt(8)` | $2\sqrt{2}$ |
| `1/sqrt(2)` | $\sqrt{2}/2$ |
| `pi*2` | $2\pi$ |
| `1/3` | a stacked one-third |
| `sin(pi/3)` | $\sqrt{3}/2$ |

The special-angle trig forms work in both RADIAN and DEGREE mode.

To get the decimal instead, either press **`Alt+ENTER`**, or add **`>dec`** to
the end of the expression. On an empty input line, `Alt+ENTER` re-runs the last
exact result as a decimal, which saves retyping it.

The reverse also exists: **`>frac`** asks for a fraction, and gives one when the
denominator is 10000 or less.

## Ans, and the history

`ans` holds the last result, so you can build on it:

```
5*4                        20
ans+2                      22
```

`ans` is an ordinary variable you name explicitly — unlike some TI models,
starting a line with an operator does not insert it for you.

Past entries are kept and can be recalled:

| Key | Action |
|-----|--------|
| `UP` on an empty line | Recall the last entry |
| `UP` / `DOWN` | Walk back and forward through past entries |
| `Alt+UP` / `Alt+DOWN` (or `Ctrl+`) | Scroll the history *view* without changing the input line |

The distinction in that last row matters: the first two keys change what is in
the input line, while `Alt`/`Ctrl` versions just scroll the display so you can
read something further up.

History survives a power cycle — it is stored on the SD card. `cls` clears the
screen but keeps the history; `clrhist` erases the history itself.

## Display formats

The MODE screen (`F3`) sets how numbers are shown:

| Format | Behaviour |
|---|---|
| FLOAT | As many significant digits as are useful |
| FIX | A fixed number of decimal places, which you choose |
| SCI | Scientific notation |
| ENG | Engineering notation — exponents are multiples of 3 |

This changes the display only. The stored value keeps full precision, so
switching to FIX 2 and back loses nothing.

## Numbers

Ordinary decimals work as expected. For very large or small values, `e` or `E`
in a literal is exponent notation: `1e10` and `1E10` both mean $10^{10}$.

Note that this is different from the constant `e` (Euler's number) standing on
its own — `e` alone is $2.718\ldots$, while `1e10` is a single numeric literal.

## What next

- Storing results into variables: [Variables and storage](02-variables-and-storage.md)
- The functions you can call: [Functions and the catalog](03-functions-and-catalog.md)
- Full syntax notes: [Syntax reference](../reference/syntax-reference.md)
