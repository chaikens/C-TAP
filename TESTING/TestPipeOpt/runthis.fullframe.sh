killall xterm

#This script expects all software and data are (linked) in the curr. working dir. (CWD). 
#for example N884A6_ch1_main_20250218200000_20250218210000a.mp4

# We support differently configured runs on the same movie.
# To distinguish such different results, differntly named
# symbolic links are made that that MASTER_MOVIE
# and their names NAME_i identify the corresponding files
# NAME_i.log.n, NAME_i.int, NAME_i.out, (eventually NAME_i.MOV)
# 
MASTER_MOVIE=../../DroneShort1.mp4

### The MOVIE_FILE below should be or will be a symbolic link,
# so the deleting code below doesn't do any harm...
# Name it to inform us about your experimental run:
#OK diff brother is DS1HalfDecimatedYUVTablePipe.mpy
MOVIE_FILE=DS1FullFrameYUVTablePipe.mp4

#Logs and the result files will carry that name.

###rm -f $MOVIE_FILE #how to rid yourself of symlinks, even those that don't exist yet.

ln -s $MASTER_MOVIE $MOVIE_FILE
if [ $? != 0 ]
then
    echo "Ignore this error when repeating"
fi

#we use multiply named symlinks to the test movie
#(For now DroneShort1.mp4 in C-TAP dir.)
#so we can distinguish different tests, since
#logs and output files are named after the---:

MOVIE_BASE=${MOVIE_FILE%.*} #Basename without the DOT

#################code for naming and creating the LOG for this run##############
touch $MOVIE_BASE.log.0 #that one's a dummy, so the next
#numbered one can be computed the first time.

logn=$( ls $MOVIE_BASE.log.* | sed s/$MOVIE_BASE.log.// | sort -n | tail -n 1)
((logn++))
LOG=$MOVIE_BASE.log.$logn

#It's a good idea to first commit the scripts, programs, settings, etc to be tested.
#   Of course, the test results will go into the next commit, but at least
#   the state before the experiment is archived.
gitcommit=$(git log HEAD -n 1 | sed -n  's/^\(commit.\{9\}\).*/\1/p')
echo $0 | cat >>$LOG
echo $( date ) | cat >>$LOG
echo $gitcommit | cat >>$LOG
git remote --verbose | cat >>$LOG #Worldwide readers of the log can get the software!

xterm -geometry 100x50+1300+0 -sb -title 'C-TAP LOG' -e tail -f $LOG &

################################################################################

make -C ../.. &>> $LOG #ensure filters are up to date. -C means do it in specified dir.
makeRet=$?
if [ $makeRet != 0 ]
then
    echo Make at toplevel returned error code $makeRet. | cat >>$LOG
    echo Make at toplevel returned error code $makeRet.
    echo Check this out before continuing.  Here is the log.
    cat $LOG
    exit 1
fi


####################################################################################
#    HERE IS WHERE YOU CONFIGURE YOUR EXPERIMENT (except for movie choice above)   #
#    You must code the configuration in a helpfully named script to do one movie   #
#    as below.  Currently, our examples like DoOneMovieHalfDecimated1ab.sh         #
#    execute short configuration settings (via bash function definitions for       #
#    how ffmpeg extracts YUV frames); and then (bash) source (aka . <a file>)      #
#    the longer common script code.                                                #
#                                                                                  #
#    Your run explanation here will go into the log.                               #
####################################################################################

##### Explain your run into the log ###
#OK diff
echo "ffmpeg .mp4---->Full frame (default) .yuv raw " >>$LOG
echo "(our table based YUVtoBMP pipeline)----->.bmp sequence -----> (Phase1aPipe)" | cat >>$LOG
echo " (redirects to/from files) > movie.int > (Phase1bPipe) > file.out " | cat >>$LOG
echo This is coded by DoOneMovieFullFrames1ab.sh | cat >>$LOG 

cat /dev/null > $MOVIE_BASE.int #empty it and guarantee tail will find it.
xterm -geometry 180x30+0+180 -title 'Phase 1a (.int file) output'  -e tail -f $MOVIE_BASE.int &

cat /dev/null > ffmpeg.log
xterm -geometry 150x46+0+0 -title 'ffmpeg mp4->raw yuv stream' -e tail -f ffmpeg.log &

#########   Code what to do here to go in our  time command ########################
#                                                                                  #
DO_ONE_MOVIE_SHELL_SCRIPT=./DoOneMovieFullFrames1ab.sh                             #
#                                                                                  #
####################################################################################
/usr/bin/time -o ${MOVIE_BASE}.time \
     -f 'real    %U\nuser  %E\nsys      %S' \
     ${DO_ONE_MOVIE_SHELL_SCRIPT} \
     $MOVIE_FILE $LOG &
####################################################################################

wait %/usr/bin/time

kill %2 %3 #the above 2 xterms

#These reports come out fast..
xterm -geometry 80x80+0+0 -sb -title 'FIRST Phase1b .out' -e less -f $MOVIE_BASE.out &

#echo "To see progress logs, run"
#echo "less ffmpeg.log, and/or"
#echo "less ${MOVIE_BASE}.int"

echo 
echo Time Report
cat ${MOVIE_BASE}.time
echo 
echo "In case of errors, (update maybe and)"
echo "use ./killLeftovers.sh to kill our residual processes."
echo
echo "View the log and results in xterm windows"
echo "(Get rid of them by typing q, or closing the window)"
echo
echo See measages from this run in
echo $LOG
echo
echo "Goodbye.  That felt good.  Thanks for the run.  See you next time."
