# 13. Matrices

Ten matrix variables, `[A]` through `[J]`. Unlike ordinary variables these are
uppercase and written in brackets, which is also how you refer to them inside an
expression.

## Entering a matrix

**As a literal**, rows in brackets, no comma between rows:

```
[[1,2][3,4]]->[A]
```

**Or with the editor**: type `mat` (or `matrix`) on the home screen, pick a
slot, set its dimensions and fill in the cells.

## Reading and writing elements

`[A](row, col)`, **1-based**:

```
[A](1,2)                   the first row, second column
```

## Arithmetic

`+`, `-` and `*` work between matrices of compatible shape, and a scalar
multiplies every element. Matrix multiplication is matrix multiplication, not
element-wise — `[A]*[B]` requires the columns of `[A]` to match the rows of
`[B]`, and gives `Dim mismatch` if they do not.

`[A]^-1` is the inverse, and `inverse([A])` is the same thing spelled out.

**There is no matrix division.** `[A]/[B]` is refused with `Matrix division:
use ^-1`, because "divide by a matrix" is ambiguous — $A B^{-1}$ and
$B^{-1} A$ are different matrices. Write the one you mean.

## Functions

| Function | Gives |
|---|---|
| `det([A])` | Determinant |
| `inverse([A])` | Inverse |
| `transpose([A])` | Transpose |
| `rref([A])` | Reduced row echelon form |
| `ref([A])` | Row echelon form |
| `rank([A])` | Rank |
| `identity(n)` | The $n \times n$ identity |
| `augment([A],[B])` | The two side by side, columns concatenated |
| `dim([A])` | `{rows, cols}` as a list |
| `eigenvals([A])`, `eig([A])` | Eigenvalues |
| `eigenvec([A])` | Eigenvectors, as columns |

`dim` returning a **list** is convenient — it drops straight into list
arithmetic, so `sum(dim([A]))` or indexing it works without conversion.

## Solving linear systems

Two routes, and they answer slightly different questions.

**Inverse**, when the system is square and non-singular:

$$A\mathbf{x} = \mathbf{b} \quad\Rightarrow\quad \mathbf{x} = A^{-1}\mathbf{b}$$

```
inverse([A])*[B]
```

**Row reduction**, which also handles systems that are inconsistent or have
infinitely many solutions:

```
rref(augment([A],[B]))
```

`augment` builds the coefficient matrix with the right-hand side attached, and
`rref` reduces it. Reading the result tells you which case you are in, where
the inverse route can only fail with `Singular matrix`.

## Eigenvalues and eigenvectors

`eigenvals([A])` gives the eigenvalues; `eigenvec([A])` gives the eigenvectors
as the columns of a matrix.

Complex spectra are supported — a rotation matrix has complex eigenvalues, and
in `a+bi` number mode you will see them rather than an error. See
[Complex numbers](14-complex-numbers.md).

## Lists and matrices

| Function | Direction |
|---|---|
| `list2mat(l1,l2)` | Lists become the columns of a matrix |
| `mat2list([A],l1)` | Matrix columns become lists |

Useful for moving data between the statistics tools, which work on lists, and
linear algebra, which works on matrices.

## Errors you may meet

| Message | Means |
|---|---|
| `Dim mismatch` | The shapes do not permit that operation |
| `Singular matrix` | No inverse exists — the determinant is zero |
| `Expected a matrix` | A matrix argument was given something else |
| `Expected (row, col)` | Element access needs two indices |
| `Matrix too large` | Beyond the size the calculator holds |
| `Matrix literal too large` | The typed literal exceeds that limit |
