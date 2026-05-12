echo SOURCED: code-ONEMOVIE.h.sh

echo "We got moviePrefix=$moviePrefix"
echo "We got movie_file=$movie_file"

echo "Making Mick West redundant, processing ONI FOIA files..."
FileName="${moviePrefix}.${ext}"
echo "FileName=$FileName"


COMMAND_ARCHIVE_PATHNAME=${RESULTS_DIR}/${moviePrefix}.cmds
cat /dev/null > ${COMMAND_ARCHIVE_PATHNAME}    #So we can just append anytime

#
# PipeOpt version logging edited to use v1 vars.
#
#MOVIE_BASE=${MOVIE_FILE%.*} #Basename without the DOT
#should be moviePrefix

#################code for naming and creating the LOG for this run##############
touch "${RESULTS_DIR}/$moviePrefix.log.0" #that one's a dummy, so the next
#numbered one can be computed the first time.

pushd ${RESULTS_DIR} > /dev/null
logn=$( ls $moviePrefix.log.* | sed s/${moviePrefix}.log.// | sort -n | tail -n 1)
popd > /dev/null
((logn++))

LOG=${RESULTS_DIR}/$moviePrefix.log.$logn
#Devel: Idea make copies of .int, .cmds, and .out (ffmpeg.output too?) with .ext.$logn

cat /dev/null > $LOG #start with an empty log.

start_time_one_movie=$(uptimenow)
echo Uptime we started on $movie_file is
echo $start_time_one_movie seconds.
echo "TIME:" $start_time_one_movie seconds. >>$LOG

#if [ ${RUN_EXPLANATION}"X" != "X" ]
#then
   echo
   echo > $LOG
   echo Your run explanation:
   echo "INFO:" Your run explanation: > $LOG
   echo
   echo > $LOG
   echo ${RUN_EXPLANATION}
   echo "INFO:" ${RUN_EXPLANATION} > $LOG
#fi

#It's a good idea to first commit the scripts, programs, settings, etc to be tested.
#   Of course, the test results will go into the next commit, but at least
#   the state before the experiment is archived.
gitcommit=$(git log HEAD -n 1 | sed -n  's/^\(commit.\{9\}\).*/\1/p')
echo "CMD:" $0 >>$LOG
echo "TIME:" $( date )  >>$LOG
echo "PGM:" $gitcommit  >>$LOG
( echo "PGM:"; git remote --verbose )  >>$LOG #Worldwide readers of the log can get the software!

xterm -geometry 150x80+1000+0 -sb -title 'C-TAP LOG' -e tail -f $LOG &
xterm_pids+=($!) #for killing 'em
################################################################################

RESULT_OF_1a_BASE="${moviePrefix}.int"
cat /dev/null > ${RESULTS_DIR}/${RESULT_OF_1a_BASE}
#Phase1a used to be will run multiple times, appending each time,
# (but the script now only does one run.)
# If we did staged runs, we must start with nothing.
# Also, this ensures xterm's tail doesn't fail.

xterm -geometry 150x30+0+500 -title 'Phase 1a (.int file) output'  -e tail -f ${RESULTS_DIR}/${RESULT_OF_1a_BASE} -s 0.1 &
xterm_pids+=($!) #for killing 'em

#That's where the C++ image processors expect us to be

cat /dev/null > ${RESULTS_DIR}/${RESULT_OF_1a_BASE}
#So stage loop can append results.

echo "Repaying TTSA investors, straightening Uri Gellar's spoons..."

#########################################################
if [ $ARCHITECTURE = "framefile" ]
then
    source "${SOFTWARE_DIR}/code-FRAMEFILE-EXTRACT-1A.h.sh"
elif [ $ARCHITECTURE = "pipeline" ]
then
     source "${SOFTWARE_DIR}/code-PIPELINE-EXTRACT-1A.h.sh"
else
    echo Unrecognized ARCHITECTURE $ARCHITECTURE
    exit 1;
fi

#Someday there might be comment or other extra stuff in files like this.
ndiffs=$(wc -l ${RESULTS_DIR}/${RESULT_OF_1a_BASE})


phase1b_start_time=$(uptimenow)
echo
echo Beginning Phase1b at $phase1b_start_time sec.
echo

RESULT_OF_1b_BASE="${moviePrefix}.out"
cat /dev/null > ${RESULTS_DIR}/${RESULT_OF_1b_BASE}
    
#so xterm's less doesnt fess. We use less since the result is finished fast.
xterm -geometry 80x80+0+0 -sb -title 'Phase1b .out' -e less -f ${RESULTS_DIR}/${RESULT_OF_1b_BASE} &
xterm_pids+=($!) #for killing 'em

#
# level for Phase1b first try is 0.5, here----------------------------------------------V---
#
PHASE1B_PARAM=0.5
Phase1b_cmd="${SOFTWARE_DIR}/$Phase1b ${RESULTS_DIR}/${RESULT_OF_1a_BASE} $ndiffs ${PHASE1B_PARAM} "
Phase1b_cmd="${Phase1b_cmd} ${OPT_CamSett} "
Phase1b_cmd="${Phase1b_cmd} > ${RESULTS_DIR}/${RESULT_OF_1b_BASE} 2>>$LOG"

echo Running
echo ${Phase1b_cmd} 
( echo ; echo ${Phase1b_cmd} ) | cat >>${COMMAND_ARCHIVE_PATHNAME}
#HUH? commanding ${Phase1b_cmd} makes some shell fail to redirect stdout!
#${Phase1b_cmd} > ${RESULT_DIR}/${RESULT_OF_1b_BASE}

(echo "CMD:" ; echo "CMD:" "First 1b try command:") | cat >> $LOG 

echo "CMD:" ${Phase1b_cmd} >> $LOG
echo >> $LOG
echo "INFO:" Phase1b report: >> $LOG
echo >> $LOG
eval ${Phase1b_cmd} 
err=$?
if [ ${err} != 0 ]
then
    echo "Phase1b run returned error code $err"
    echo "$0 running in cwd=" $(pwd)
    #echo "$0 will continue and do a redo with level=0.98 instead of 0.5"
    echo "$0 will exit.  See cmd above^^."
    exit 1
fi
n=`cat ${RESULTS_DIR}/${RESULT_OF_1b_BASE} | wc -l`
echo
echo "First try of Phase1b reported $n frames have objects, see the lines in:"
echo ${RESULTS_DIR}/${RESULT_OF_1b_BASE}
echo

if [ $n -gt 50000 ] || [ $n -eq 0 ]
then
    if [ $n == 0 ]
    then
	echo "Part deux: electric boogaloo... First try found 0 frames with objects."
	echo "INFO:" "Part deux: electric boogaloo... First try found 0 frames with objects." >> $LOG
	
	#we don't redo Phase1a since current version ignores cloud cover param."
	#	    echo "We will redo Phase1a"
	#
	#	    u=0; v=0
	#	    w=30
	#           t=$((nframes/w))
	#           while [ $u -lt $nframes ]
	#            do
	#	        #not updated below
	#		${SOFTWARE_DIR}/$Phase1a $u $t 1 >> ${RESULTS_DIR}/${RESULT_OF_1a_BASE} 2> /dev/null 
	#		((++v))
	#		echo Phase 1Bs 1A redo: Part $v of $w Done
	#		if [ $v -gt $w ]
	#		then
	#                   echo "Don't panic -- there was a remainder from the division!"
	#		u=$((u+t))
	#		if [ $((u+t)) -gt $nframes ]
	#		then
	#                   t=$((nframes-u))
	#		fi
	#            done
	#


	#
	# level for Phase1b redo is 0.98, here-----------------------------------------------------------V---
	#
	PHASE1B_REDO_PARAM=0.98
	Phase1b_redo_cmd="${SOFTWARE_DIR}/$Phase1b ${RESULTS_DIR}/${RESULT_OF_1a_BASE} $ndiffs ${PHASE1B_REDO_PARAM} "
	Phase1b_redo_cmd="${Phase1b_redo_cmd} ${OPT_CamSett} "
	Phase1b_redo_cmd="${Phase1b_redo_cmd} > ${RESULTS_DIR}/${RESULT_OF_1b_BASE} 2>>$LOG"
	( echo  ; echo "${Phase1b_redo_cmd}" ) | cat >> ${COMMAND_ARCHIVE_PATHNAME}	
	(echo "CMD:" ; echo "CMD:" "${Phase1b_redo_cmd}" ; echo Report ) | cat >> $LOG
	eval $Phase1b_redo_cmd
	err=$?
	phase1b_finish_time=$(uptimenow)
	if [ ${err} != 0 ]
	then
	    echo "Phase1b redo run returned error code $err"
	    echo "$0 running in cwd=" $(pwd)
	    #echo "$0 will continue and do a redo with level=0.98 instead of 0.5"
	    echo "$0 will exit.  See cmd above^^."
	    exit 1
	fi

	
	n=`cat ${RESULTS_DIR}/${RESULT_OF_1b_BASE} | wc -l`
	echo "Phase 1B: Re-Done reports $n frames with objects."
	echo "INFO:" "Phase 1B: Re-Done reports $n frames with objects." >> $LOG
	echo "TIME:" "Phase 1B finished at $phase1b_finish_time sec" >> $LOG
	echo "TIME:" "Took $(numdif $phase1b_finish_time $phase1b_start_time) sec." >> $LOG
    fi
fi
    
#now, after either one or a redo of Phase1b,
# don't waste time on empty file OR overloading your HD
if [ $n -gt 465000 ] || [ $n -eq 0 ]
then
    (echo "Either too many or too few objects..." ; echo "On to the next movie, if any" )
    (echo "INFO:" "Either too many or too few objects..." ; echo "On to the next movie, if any" ) >> $LOG

    finish_time_one_movie=$(uptimenow)
    echo Finished movie $movie_file at
    echo uptime $finish_time_one_movie seconds.
    echo "TIME:" finish at $finish_time_one_movie seconds. >> $LOG
    et=$(numdif $finish_time_one_movie $start_time_one_movie)
    echo that took $et wall clock seconds.
    echo "TIME:" net $et seconds. >> $LOG
    continue #top level movie loop
fi
# number above assumes 2.9 MB/image: (2.9*(46500+216e3))/1e3 = 760 GB of free space needed! Adjust for your machine


if [ $MAKE_BABY_MOVIE = "yes" ]
then
    echo "Logging range, deploying Little Green Men..circling 'em in picnn.bmps"
    source "${SOFTWARE_DIR}/code-BMP-BABYMOVIE.h.sh"
else
    echo "No baby movie today, someone said no, or our pipeline can't do it yet."
fi

# We're done with I/O thru the bitmaps dir
pushd ${SOFTWARE_DIR} > /dev/null

# save the result movie
    
    
#echo "Cleaning detritus..." #I'd rather leave that for the build system.

#rm -rf *.cpp~ *.sh~ *.txt~ *.err bitmaps/
# sdc: Keep old bitmaps BUT the script removes them early next time
# because the C++ programs process all files in their cwd.

#rm -f *.cpp~ *.sh~ *.txt~ 

if [ $COPYMOVIES ]
then
    echo "about to delete input movie" $movie_file
    echo "STEP:" "about to delete input movie" $movie_file >> $LOG
    rm $movie_file
fi

popd > /dev/null

echo "Cosmic consciousness has been achieved!!"
echo "from yet another movie $o of $numMovieFiles."

finish_time_one_movie=$(uptimenow)
echo Finished movie $movie_file at
echo uptime $finish_time_one_movie seconds.
echo "TIME:" finish at $finish_time_one_movie seconds. >> $LOG
et=$(numdif $finish_time_one_movie $start_time_one_movie)
echo that took $et wall clock seconds.
echo "TIME:" net $et seconds. >> $LOG
