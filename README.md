
[![Travis build
status](https://travis-ci.org/SymbolixAU/jsonify.svg?branch=master)](https://travis-ci.org/SymbolixAU/jsonify)
[![Coverage
status](https://codecov.io/gh/SymbolixAU/jsonify/branch/master/graph/badge.svg)](https://codecov.io/github/SymbolixAU/jsonify?branch=master)

# jsonify

jsonify converts a data.frame to JSON.

### Aren’t there already data.frame to JSON converters?

Yep.

### So why did you build this one?

Because I wanted it available at the source ( C++ ) level for
integrating into other packages.

### Is it fast

It does alright

``` r

library(microbenchmark)
library(jsonlite)

n <- 1e6
df <- data.frame(
  id = 1:n
  , value = sample(letters, size = n, replace = T)
  , val2 = rnorm(n = n)
  , log = sample(c(T,F), size = n, replace = T)
  #, dte = sample(seq(as.Date("2018-01-01"), as.Date("2018-01-31"), length.out = n), size = n, replace = T)
)

microbenchmark(
  jsonlite = {
    js <- jsonlite::toJSON( df )
  },
  jsonify = {
    js <- jsonify::to_json( df )
  },
  times = 5
)
#  Unit: milliseconds
#       expr       min        lq     mean   median       uq     max neval
#   jsonlite 2050.6168 2428.0362 2596.670 2585.298 2675.907 3243.49     5
#    jsonify  938.8224  945.1567 1077.529 1065.923 1150.350 1287.39     5

n <- 1e7
x <- rnorm(n = n)
microbenchmark(
  jsonlite = {
    js <- jsonlite::toJSON( x )
  },
  jsonify = {
    js <- jsonify::to_json( x )
  },
  times = 5
)
#  Unit: seconds
#       expr      min       lq     mean   median       uq      max neval
#   jsonlite 1.533383 1.606570 1.617233 1.617311 1.645076 1.683823     5
#    jsonify 1.604660 1.627856 1.719328 1.643991 1.716037 2.004097     5
```

### What doesn’t it do?

Quite a lot, it’s still in development\!

### There’s no ‘Date’ type in JSON, how have you handled this?

I’ve currently left this up to the user. At its core in R, `Dates` are
numeric, so they’re treated as numbers when converted to JSON.

I’ve given the option to the user to coerce to character through the
`numeric_dates` argument.

``` r
df <- data.frame(dte = as.Date("2018-01-01"))
jsonify::to_json( df )
#  [1] "[{\"dte\":17532.0}]"

df <- data.frame(dte = as.Date("2018-01-01"))
jsonify::to_json( df, numeric_dates = FALSE )
#  [1] "[{\"dte\":\"2018-01-01\"}]"

df <- data.frame(dte = as.POSIXct("2018-01-01 01:01:01"))
jsonify::to_json( df )
#  [1] "[{\"dte\":1514728861.0}]"

df <- data.frame(dte = as.POSIXct("2018-01-01 01:01:01"))
jsonify::to_json( df, numeric_dates = FALSE )
#  [1] "[{\"dte\":\"2018-01-01 01:01:01\"}]"
```

### What do you mean by ‘available at the source’ ?

I want to be able to call the C++ code from another package, without
going to & from R. Therefore, the C++ code is imlemented in headers, so
you can `LinkTo` it in your own package.

For example, the LinkingTo section in DESCRIPTION will look something
like

``` r
LinkingTo: 
    Rcpp,
    jsonify
```

And in a c++ source file so you can `#include` the header and use the
available functions

``` cpp
#include "jsonify.hpp"
// [[Rcpp::depends(jsonify)]]

Rcpp::StringVector my_json( Rcpp::DataFrame df ) {
  return jsonify::to_json( df );
}
```

### Can I call it from R if I want to?

Yes.

``` r
df <- data.frame(
  id = 1:3
  , val = letters[1:3]
  , stringsAsFactors = FALSE 
  )
jsonify::to_json( df )
#  [1] "[{\"id\":1,\"val\":\"a\"},{\"id\":2,\"val\":\"b\"},{\"id\":3,\"val\":\"c\"}]"
```

### How do I install it?

Install the development version from [GitHub](https://github.com/) with:

``` r
# install.packages("devtools")
devtools::install_github("SymbolixAU/jsonify")
```
