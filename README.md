
KiR_Kiel.Reduction.Language 
Forked  form "ftp.informatik.uni-kiel.de/".


This directory contains the first official distribution of
reduction machine pi-red:

pi-red is a complex software system for developing and evaluating
programs in a functional language. It provides a sophisticated
syntax-directed editor and a complete run-time environment for
functional languages featuring the reduction semantics of an applied
lambda-calculus. Both programming and program execution are supported
as fully interactive processes. Programs may be constructed either
top-down or bottom-up by systematically inserting expressions into
place-holder positions of other expressions or by replacing expressions
with others. Program execution is realized as a sequence of
meaning-preserving program transformations based on a fixed set of
rewrite (or reduction) rules, of which the most important one is a
full-fledged beta-reduction. Programs  may either be run to completion
`in one shot' or be reduced in a step-by-step mode, with intermediate
programs being displayed to the user for inspection and modifications.
Due to the availability of a full-fledged lambda-calculus, pi-red fully
supports higher-order functions: functions may be freely applied to
other functions or to themselves, and new functions resulting from the
evaluation of (partial) function applications may be made visible to
the user in high-level notation.

Included in the distribution is the complete interactive programming
environment (editor, preprocessor, reduction system simulator,
postprocessor and online help), the pre-release of the KiR-Manual as
well as installation instructions so you should be able to install it
easily on Unix Workstations (tested and preconfigured for Sun, Apollo,
HP and NeXT)

  manual-1.2    documents the language KiR and the use of the
		reduction machines
  lneu-0.4.1    source code for the interpreting version pi-red^*
		(includes manual)
  red-0.6       source code for the compiling version pi-red^+

The archives are packed using tar and GNU gzip.

University of Kiel, Department for Computer Science, Prof. Dr. W. Kluge
Email: base@informatik.uni-kiel.d400.de
