# Syntax Reference

> **Generated file — do not edit by hand.** Regenerate with
> `python3 scripts/gen-doc-reference.py` (source: `src/apps/help_screen.cpp (kSyntaxLines)`).

Mirrors the on-device `F5` HELP screen's tab of the same name.
Lines are curated for a narrow on-device display column; wrapped
continuations (marked by a leading space in the source) are
rejoined below, everything else is kept as its own line verbatim.

## CASE

- `input is case-sensitive:`
- `functions, vars, commands`
- `are all lowercase`

## STORE

- `expr->a     store result in a`
- `works for a-z and theta`

## CONSTANTS

- `pi, e (Euler's number)`
- `e is not a variable; 1e10 or`
- `1E10 = scientific literal`

## VARIABLES

- `a-z, theta, ans (lowercase)`
- `ans = last result`

## FACTORIAL

- `n! or fac(n)`

## DISPLAY

- `MODE > Display: FLOAT/FIX/`
- `SCI/ENG (eng: exp mult of 3)`
- `expr>frac shows a fraction`
- `(den <= 10000), >dec decimal`

## ANGLE MODE

- `MODE sets RADIAN or DEGREE;`
- `trig functions follow it`

## GRAPH MODES

- `MODE > Graph mode:`
- `FUNC / PARAM / POLAR / SEQ`
- `PARAM plots X1T(t), Y1T(t)`
- `over Tmin..Tmax (see WINDOW)`
- `POLAR plots r(theta) over`
- `THmin..THmax; angle mode`
- `applies to theta`
- `SEQ plots u/v/w(n): e.g.`
- `u(n)=u(n-1)+1 with seed`
- `u(nMin); lags n-1, n-2;`
- `MODE > Seq plot: TIME/WEB`

## POWER (settings cmd)

- `LCD/kbd brightness levels;`
- `auto power-down dims after`
- `N min, any key wakes`

## HISTORY

- `UP on empty input recalls;`
- `UP/DOWN walks past entries`

## LISTS (l1..l6)

- `{1,2,3}->l1  store a list`
- `l1+2*l2      element-wise`
- `{1,2,3}+2    literals too`
- `range(1,9)   list 1..9`
- `range(0,1,.1) with step`
- `sum/prod/length(l) scalar`
- `mean/median/stdev(l)`
- `args: l1..l6, range(...),`
- `{...} or any list expr`
- `sort_asc(l1) sorts in place`
- `cumsum(l1), delta_list(l1)`
- `seq(x^2,x,1,10,1)->l1`

## NAMED LISTS

- `{1,2}->costs  creates it`
- `(2-5 chars, letter first)`
- `usable anywhere l1 works;`
- `editor: Alt+N new, Alt+R`
- `rename, Alt+X delete`
- `type list(s) for the editor`

## STATS (stat cmd)

- `1-Var/2-Var over l1..l6,`
- `optional freq list (1-Var)`
- `10 regressions: Lin Quad`
- `Cubic Quart Ln Exp Pwr`
- `Logistic Sin Med-Med`
- `Store to fills a y slot`
- `with the fitted model`

## DISTRIBUTIONS (dist cmd)

- `normal, t, chisq, f: _pdf, _cdf(lo,hi,..), _inv`
- `cdf is P(lo<=X<=hi); use`
- `-1e99/1e99 for open tails`
- `inv takes lower-tail area`
- `binomial, poisson,`
- `geometric: _pmf, _cdf(k..)`
- `k, n must be integers`
- `see FUNC tab for signatures`
