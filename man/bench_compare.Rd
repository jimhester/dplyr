\name{bench_compare}
\alias{bench_compare}
\alias{bench_tbls}
\alias{compare_tbls}
\alias{eval_tbls}
\title{Evaluate, compare, benchmark operations of a set of srcs.}
\usage{
  bench_tbls(tbls, op, ..., times = 10)

  compare_tbls(tbls, op, ref = NULL,
    compare = equal_data_frame, ...)

  eval_tbls(tbls, op)
}
\arguments{
  \item{tbls}{A list of \code{\link{tbl}}s.}

  \item{op}{A function with a single argument, called often
  with each element of \code{tbls}.}

  \item{ref}{For checking, an data frame to test results
  against. If not supplied, defaults to the results from
  the first \code{src}.}

  \item{compare}{A function used to compare the results.
  Defaults to \code{equal_data_frame} which ignores the
  order of rows and columns.}

  \item{times}{For benchmarking, the number of times each
  operation is repeated.}

  \item{\dots}{For \code{compare_tbls}: additional
  parameters passed on the \code{compare} function

  For \code{bench_tbls}: additional benchmarks to run.}
}
\value{
  \code{eval_tbls}: a list of data frames.

  \code{compare_tbls}: an invisible \code{TRUE} on success,
  otherwise an error is thrown.

  \code{bench_tbls}: an object of class
  \code{\link[microbenchmark]{microbenchmark}}
}
\description{
  These functions support the comparison of results and
  timings across multiple sources.
}
\examples{
if (require("Lahman") && require("microbenchmark")) {
lahman_local <- lahman_srcs("df", "dt", "cpp")
teams <- lapply(lahman_local, function(x) x \%.\% tbl("Teams"))

compare_tbls(teams, function(x) x \%.\% filter(yearID == 2010))
bench_tbls(teams, function(x) x \%.\% filter(yearID == 2010))

# You can also supply arbitrary additional arguments to bench_tbls
# if there are other operations you'd like to compare.
bench_tbls(teams, function(x) x \%.\% filter(yearID == 2010),
   base = subset(Teams, yearID == 2010))

# A more complicated example using multiple tables
setup <- function(src) {
  list(
    src \%.\% tbl("Batting") \%.\% filter(stint == 1) \%.\% select(playerID:H),
    src \%.\% tbl("Master") \%.\% select(playerID, birthYear)
  )
}
two_tables <- lapply(lahman_local, setup)

op <- function(tbls) {
  semi_join(tbls[[1]], tbls[[2]], by = "playerID")
}
# compare_tbls(two_tables, op)
bench_tbls(two_tables, op, times = 2)

}
}
\seealso{
  \code{\link{src_local}} for working with local data
}

