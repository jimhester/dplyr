\name{setops-data.frame}
\alias{intersect.data.frame}
\alias{setdiff.data.frame}
\alias{setequal.data.frame}
\alias{setops-data.frame}
\alias{union.data.frame}
\title{Set operations for data frames.}
\usage{
  \method{intersect}{data.frame} (x, y, ...)

  \method{union}{data.frame} (x, y, ...)

  \method{setdiff}{data.frame} (x, y, ...)

  \method{setequal}{data.frame} (x, y, ...)
}
\arguments{
  \item{x,y}{Two data frames to compare, igoring order of
  row and columns}

  \item{...}{Needed for compatibility with generic.
  Otherwise ignored.}
}
\description{
  These set operations are implemented with an efficeint
  C++ backend.
}
\examples{
mtcars$model <- rownames(mtcars)
first <- mtcars[1:20, ]
second <- mtcars[10:32, ]

intersect(first, second)
union(first, second)
setdiff(first, second)
setdiff(second, first)

setequal(mtcars, mtcars[32:1, ])
}

