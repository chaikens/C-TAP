# TESTING/TestPipeline

Instructions

(0) Try this setup the first time by running
./runthis.halfdecimated.sh and ./runthis.fullframe.sh
Observe the results as instucted by the scripts' console
output. 

(1) Do updates and test any updates you want.

(2) Note that names of symbolic links to a common .mp4
movie are used to name and distinguish test setups.

(3) add and commit until you have a setup for which
you want to do tests good enough for you to analyze
the results. See runthis.fullframe.sh and runthis.halfdecimated.sh
as examples with included instructions for configuring the
setup. 

(4) run your version of ./runthis.fullframe.sh 
or ./runthis.halfdecimated.sh  The resulting movie_name.log.n file
contains the commit containing the setup.  (Of course, this assumes
you made no uncommitted changes.)

(5) You have created a movie name, say MOVIE-NAME.mp4, when you
configured a test.  The result files are MOVIE-NAME.int
and MOVIE-NAME.out The log file is MOVIE-NAME.log.n where
n is the largest (decimal) number among files with this log file
pattern.  

(6) To archive your test, add the MOVIE-NAME.int, .out and .log.n
and commit, with a message describing the test.

(7) Repeat until done.


