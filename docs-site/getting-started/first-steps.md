# First steps

You have flashed the firmware and you are looking at the home screen. This page
gets you oriented in about five minutes.

## Do a calculation

Type an expression and press `ENTER`.

```
2+3*4                      14
```

Precedence works as you would expect: `*` and `/` before `+` and `-`, and `^`
before both. `^` is right-associative, so `2^3^2` is $2^{(3^2)} = 512$.

**Everything is lowercase.** Function names, variables and commands are all
lowercase, and input is case-sensitive — `SIN(0)` is a syntax error, `sin(0)`
is 0.

## Exact answers

Try `sqrt(8)`. You get $2\sqrt{2}$ in amber, not a decimal — when a result has a
clean closed form, the calculator shows that form.

Press **`Alt+ENTER`** for the decimal, or add `>dec` to the expression.

## Store a value

```
2->a                       2
a*10                       20
```

Variables are the lowercase letters `a`–`z`, plus `theta`, with `ans` holding
the last result. `e` and `i` are reserved — Euler's number and the imaginary
unit.

## Recall what you typed

`UP` on an empty line brings back the last entry; `UP` and `DOWN` walk through
the history. `Alt+UP` and `Alt+DOWN` scroll the *view* without changing the
input line.

History survives a power cycle. So do your variables, graphs, lists and
matrices — there is no save step.

## The five softkeys

The same five mean the same thing on every screen:

| Key | Goes to |
|-----|---------|
| `F1` | Editor for the active graph mode (Y= / PAR / POLAR / SEQ) |
| `F2` | Window settings |
| `F3` | Mode settings |
| `F4` | Trace — opens the graph if you are elsewhere |
| `F5` | Graph, and graph ↔ table from there |

Plus `HOME` to come back here, and `ESC` to go back or cancel an edit.

`F6` is `Shift+F1` and opens the CAS menu. The keyboard turns Shift chords into
their own key codes, which is why bindings in this firmware use `Alt` and
`Ctrl` rather than Shift.

## Screens you reach by typing

Several screens have no key of their own — type their name on the home screen:

| Type | Get |
|---|---|
| `help` or `?` | The built-in help browser |
| `lists` | The list editor |
| `stats` | Statistics |
| `mat` | The matrix editor |
| `diag` | Hardware diagnostics |

`help` is the one to remember. It carries the full function catalog, a
per-screen key reference and syntax notes — generated from the same table the
expression parser uses, so it always matches the firmware you are running.

## Graph something

1. `F1` — the Y= editor.
2. Type `x^2` into `Y1`, `ENTER`.
3. `F5` — graph it.
4. `F4` — trace along it with `LEFT` and `RIGHT`.
5. `S` — reset to the standard window if you get lost.

## Where next

- [Basic calculations](../guide/01-basic-calculations.md) — the home screen in
  full
- [Graphing](../guide/04-graphing.md) — the Y= editor, colours, shading
- [Function catalog](../reference/function-catalog.md) — everything you can call
