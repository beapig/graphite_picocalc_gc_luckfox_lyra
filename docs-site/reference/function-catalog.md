# Function Catalog

> **Generated file — do not edit by hand.** Regenerate with
> `python3 scripts/gen-doc-reference.py` (source: `src/math/catalog.cpp`).

Every function the expression parser recognizes, in the same order
as the on-device `F5` HELP screen's FUNC tab (it reads this exact
table).

| Name | Signature | Description | Arity |
|---|---|---|---|
| `sin` | `sin(x)` | Sine (angle mode) | 1 |
| `cos` | `cos(x)` | Cosine (angle mode) | 1 |
| `tan` | `tan(x)` | Tangent (angle mode) | 1 |
| `asin` | `asin(x)` | Inverse sine | 1 |
| `acos` | `acos(x)` | Inverse cosine | 1 |
| `atan` | `atan(x)` | Inverse tangent | 1 |
| `log` | `log(x)` | Log base 10 | 1 |
| `ln` | `ln(x)` | Natural log | 1 |
| `fac` | `fac(n)` | Factorial (also n!) | 1 |
| `ncr` | `ncr(n, r)` | n choose r | 2 |
| `npr` | `npr(n, r)` | Permutations | 2 |
| `rand` | `rand()` | Random in [0, 1) | 0 |
| `round` | `round(x, n)` | Round to n decimals | 2 |
| `min` | `min(a, b)` | Smaller of a and b | 2 |
| `max` | `max(a, b)` | Larger of a and b | 2 |
| `deg` | `deg(x)` | Radians to degrees | 1 |
| `rad` | `rad(x)` | Degrees to radians | 1 |
| `sum` | `sum(l)` | Sum of list elements | 1 |
| `prod` | `prod(l)` | Product of elements | 1 |
| `length` | `length(l)` | List element count | 1 |
| `sort_asc` | `sort_asc(l)` | Sort list ascending | 1 |
| `sort_desc` | `sort_desc(l)` | Sort list descending | 1 |
| `cumsum` | `cumsum(l)` | Cumulative sums | 1 |
| `delta_list` | `delta_list(l)` | Pairwise differences | 1 |
| `seq` | `seq(f,v,lo,hi,st)` | Sequence into a list | 5 |
| `range` | `range(lo,hi,st?)` | List lo..hi, step 1 | 3 |
| `mean` | `mean(l)` | Mean of elements | 1 |
| `median` | `median(l)` | Median of elements | 1 |
| `stdev` | `stdev(l)` | Sample stddev (Sx) | 1 |
| `normal_pdf` | `normal_pdf(x,mu,sd)` | Normal density | 3 |
| `normal_cdf` | `normal_cdf(lo,hi,mu,sd)` | P(lo<=X<=hi) | 4 |
| `normal_inv` | `normal_inv(area,mu,sd)` | Inverse CDF | 3 |
| `t_pdf` | `t_pdf(x,df)` | Student t density | 2 |
| `t_cdf` | `t_cdf(lo,hi,df)` | t P(lo<=X<=hi) | 3 |
| `t_inv` | `t_inv(area,df)` | t inverse CDF | 2 |
| `chisq_pdf` | `chisq_pdf(x,df)` | Chi-sq density | 2 |
| `chisq_cdf` | `chisq_cdf(lo,hi,df)` | P(lo<=X<=hi) | 3 |
| `chisq_inv` | `chisq_inv(area,df)` | Chi-sq inv CDF | 2 |
| `f_pdf` | `f_pdf(x,d1,d2)` | F density | 3 |
| `f_cdf` | `f_cdf(lo,hi,d1,d2)` | F P(lo<=X<=hi) | 4 |
| `f_inv` | `f_inv(area,d1,d2)` | F inverse CDF | 3 |
| `binomial_pmf` | `binomial_pmf(k,n,p)` | P(X=k) | 3 |
| `binomial_cdf` | `binomial_cdf(k,n,p)` | P(X<=k) | 3 |
| `poisson_pmf` | `poisson_pmf(k,lam)` | P(X=k) | 2 |
| `poisson_cdf` | `poisson_cdf(k,lam)` | P(X<=k) | 2 |
| `geometric_pmf` | `geometric_pmf(k,p)` | P(1st success=k) | 2 |
| `geometric_cdf` | `geometric_cdf(k,p)` | P(X<=k) | 2 |
| `det` | `det([A])` | Determinant | 1 |
| `inverse` | `inverse([A])` | Matrix inverse | 1 |
| `transpose` | `transpose([A])` | Transpose | 1 |
| `rref` | `rref([A])` | Reduced row echelon | 1 |
| `ref` | `ref([A])` | Row echelon form | 1 |
| `rank` | `rank([A])` | Matrix rank | 1 |
| `identity` | `identity(n)` | n x n identity | 1 |
| `augment` | `augment([A],[B])` | Concat columns | 2 |
| `dim` | `dim([A])` | {rows, cols} list | 1 |
| `eigenvals` | `eigenvals([A])` | Real eigenvalues | 1 |
| `eig` | `eig([A])` | Eigenvalues (alias) | 1 |
| `eigenvec` | `eigenvec([A])` | Eigenvector columns | 1 |
| `solve` | `solve(f,x,lo,hi)` | Root of f (or guess) | 4 |
| `dot` | `dot(A,B)` | Dot product (lists) | 2 |
| `cross` | `cross(A,B)` | Cross product (3-elem) | 2 |
| `norm` | `norm(A)` | Euclid/Frobenius norm | 1 |
| `convert` | `convert(v, mi, km)` | Unit conversion | 3 |
| `list2mat` | `list2mat(l1,l2)` | Lists to columns | 2 |
| `mat2list` | `mat2list([A],l1)` | Columns to lists | 2 |
