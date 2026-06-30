echo SOURCED: code-ONEMOVIE.h.sh
#
# (1) Set up or increment, for one movie, job repetition number ($logn below)
# to be appended to the RESULTS dir files relevent to that movie.
#
# 
echo "We got moviePrefix=$moviePrefix"
echo "We got movie_file=$movie_file"   #full pathname

echo "Making Mick West redundant, processing ONI FOIA files..."
FileName="${moviePrefix}.${ext}"       #basename (last pathname component)
echo "FileName=$FileName"

#
# PipeOpt version logging edited to use v1 vars.
#
#MOVIE_BASE=${MOVIE_FILE%.*} #Basename without the DOT
#should be moviePrefix

#################code for naming and creating the LOG for this run##############
touch "${RESULTS_DIR}/$moviePrefix.log.0"
#that one's a dummy, so the next
#numbered one can be computed the first time.

#Compute the next run number and keep it as $logn and later the suffix of the .log file.
pushd ${RESULTS_DIR} > /dev/null
logn=$( ls $moviePrefix.log.* | sed s/${moviePrefix}.log.// | sort -n | tail -n 1)
popd > /dev/null
((logn++))

#store a copy of the top level script suffixed by run number.
cp $(which $0) $RESULTS_DIR/$0.$logn

#we don't yet support running the top script except in $SOFTWARE_DIR
chmod 644 $RESULTS_DIR/$0.$logn

#Make symbolic links with $logn suffix to current movies' video and frame files.
#Good for later visualization operations, to get at the original movie.
pushd ${RESULTS_DIR} > /dev/null
ln -s ${precopy_movie_file} ${FileName}.${logn}
ln -s $BITMAPS_DIR ${BITMAPS_DIR_NAME}.${logn}
popd > /dev/null

LOG=${RESULTS_DIR}/$moviePrefix.log.$logn
TIMELOG=${RESULTS_DIR}/$moviePrefix.times.$logn
time_cmd_prefix="/usr/bin/time --verbose --output=${TIMELOG} --append "
#We use /usr/bin/time instead of shell's builtin time.
#Devel: Idea make copies of .int, .cmds, and .out (ffmpeg.output too?) with .ext.$logn

#cat /dev/null > $LOG #start with an empty log.
echo FIRST LINE OF LOG > $LOG

COMMAND_ARCHIVE_PATHNAME=${RESULTS_DIR}/${moviePrefix}.cmds.$logn
cat /dev/null > ${COMMAND_ARCHIVE_PATHNAME}    #So we can just append anytime

if [ -n "${OPT_CamSett}" ]
then
    echo "INFO: Our CamSett file contents:" >> $LOG
    echo >> $LOG
    #Hack OPT_CamSett if non-null is "--CamSett-file <pathname>"
    cat ${OPT_CamSett/* /""} >> $LOG
    echo >> $LOG
fi

start_time_one_movie=$(uptimenow)
echo $(date)
echo Uptime we started on $movie_file is
echo $start_time_one_movie seconds.
echo "TIME:" $start_time_one_movie seconds. >>$LOG

if [ "${RUN_EXPLANATION}X" != "X" ]
then
   echo
   echo >> $LOG
   echo Your run explanation:
   echo
   echo "INFO:" Your run explanation: >> $LOG
   echo
   echo >> $LOG
   echo ${RUN_EXPLANATION}
   echo
   echo "INFO:" ${RUN_EXPLANATION} >> $LOG
fi

#It's a good idea to first commit the scripts, programs, settings, etc to be tested.
#   Of course, the test results will go into the next commit, but at least
#   the state before the experiment is archived.
gitcommit=$(git log HEAD -n 1 | sed -n  's/^\(commit.\{9\}\).*/\1/p')
echo "CMD:" $0 >>$LOG
echo "TIME:" $( date )  >>$LOG
echo "PGM:" $gitcommit  >>$LOG
( echo "PGM:"; git remote --verbose )  >>$LOG #Worldwide readers of the log can get the software!

if [ $KILLALL_XTERMS_AT_START"x" == "yesx" ]
then
    killall -q xterm 
fi

xterm ${XTERM_PARAM} -geometry 150x80+1000+0 -sb -title 'C-TAP LOG' -e less +F -f $LOG &
#option +F acts like user types F (continue after end of file)
xterm_pids+=" $!" #for killing 'em
################################################################################

RESULT_OF_1a_BASE="${moviePrefix}.int.${logn}"
IMAGE_MASK_BASE="${moviePrefix}.btmask.${logn}"

#This string will either name the file to write Phase 1a results in,
# or be symbolically linked to an old Phase1a results file (or symlink!)
# or be ignored, depending on the following tests

if [ ${PHASE_1a_RESULT_OLD_N_OR_NONE}xxx = xxx ]
then
    #Make a file to hold a result we will compute.
    cat /dev/null > ${RESULTS_DIR}/${RESULT_OF_1a_BASE}

    #Phase1a used to be will run multiple times, appending each time,
    # (but the script now only does one run.)
    # If we did staged runs, we must start with nothing.
    # Also, this ensures xterm's tail doesn't fail.

    #xterm -geometry 200x30+0+500 -title 'Phase 1a (.int file) output'  -e tail -f ${RESULTS_DIR}/${RESULT_OF_1a_BASE} -s 0.1 &
    xterm -bg '#E0E0FF' ${XTERM_PARAM} -geometry 200x30+0+542 -title 'Phase 1a (.int file) output'  -e less +F -f ${RESULTS_DIR}/${RESULT_OF_1a_BASE} -s 0.1 &
    xterm_pids+=" $!" #for killing 'em

    #That's where the C++ image processors expect us to be

    cat /dev/null > ${RESULTS_DIR}/${RESULT_OF_1a_BASE}
    #So stage loop can append results.
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
    #stupid old unix:  wc -l <filename> prints count<space>filename HA so we must cat here
    ndiffs=$(cat ${RESULTS_DIR}/${RESULT_OF_1a_BASE} | wc -l )
else

    if [[ $PHASE_1a_RESULT_OLD_N_OR_NONE =~ ^[0-9]+$ ]]
    then
   	#This value is decimal whole number
	#We do not do Phase 1a but make a symlink for other stages to use an old version
	pushd ${RESULTS_DIR}
	if [ -r ${moviePrefix}.int.${PHASE_1a_RESULT_OLD_N_OR_NONE} ]
	then
	    ln -s ${moviePrefix}.int.${PHASE_1a_RESULT_OLD_N_OR_NONE} ${moviePrefix}.int.${logn}
            ln -s ${moviePrefix}.btmask.${PHASE_1a_RESULT_OLD_N_OR_NONE} ${moviePrefix}.btmask.${logn}
	    popd
	else
   	    echo You are trying to reuse a Phase1a result that doesn\'t exist.
	    echo In $0
	    echo You set PHASE_1a_RESULT_OLD_N_OR_NONE to $PHASE_1a_RESULT_OLD_N_OR_NONE
	    echo but we can\'t read
	    echo ${moviePrefix}.int.${PHASE_1a_RESULT_OLD_N_OR_NONE}
	    kill_our_xterms
	    exit 1
	fi
	ndiffs=$(cat ${RESULTS_DIR}/${RESULT_OF_1a_BASE} | wc -l )
    else
	if [ ${PHASE_1a_RESULT_OLD_N_OR_NONE}Y = NONEY ]
	then
	    echo We are scripted to both no do and not use Phase1a results
	    echo It is not implemented yet to using old Phase1b results for something new.
	    echo ERROR
	    exit 1
	fi
    fi
fi
echo "Repaying TTSA investors, straightening Uri Gellar's spoons..."

#########################################################




phase1b_start_time=$(uptimenow)
echo
echo Beginning Phase1b at $phase1b_start_time sec.
echo

RESULT_OF_1b_BASE="${moviePrefix}.out.${logn}"
cat /dev/null > ${RESULTS_DIR}/${RESULT_OF_1b_BASE}
    
#so xterm's less doesnt fess. We use less since the result is finished fast.

echo DEBUG starting phase1a xterm xterm_pids is
echo $xterm_pids
xterm -bg '#FFFFE0' ${XTERM_PARAM} -geometry 80x80+0+0 -sb -title 'Phase1b .out' -e less -f +F ${RESULTS_DIR}/${RESULT_OF_1b_BASE} &
xterm_pids+=" $!" #for killing 'em

#
# level for Phase1b first try is 0.5, here----------------------------------------------V---
#

phase1b_out=${RESULTS_DIR}/${RESULT_OF_1b_BASE}
phase1b_cmd="$time_cmd_prefix ${SOFTWARE_DIR}/$phase1b "
#
# level for Phase1b was 0.5 here, now this is set by our DEFAULTS and C-TAP scripts
#                                                                --V---
phase1b_cmd+=" ${RESULTS_DIR}/${RESULT_OF_1a_BASE} $ndiffs ${PHASE_1b_LEVEL} "
phase1b_cmd+=" ${OPT_CamSett} "
phase1b_cmd+=" > $phase1b_out 2>>$LOG"

echo Running
echofold ${phase1b_cmd}
( echo ; echo ${phase1b_cmd} ) | cat >>${COMMAND_ARCHIVE_PATHNAME}
#HUH? commanding ${Phase1b_cmd} makes some shell fail to redirect stdout!
#${Phase1b_cmd} > $phase1b_out

(echo "CMD:" ; echo "CMD:" "First 1b try command:") | cat >> $LOG 

echo -n "CMD:"
echo $(echofold $phase1b_cmd) >> $LOG
echo -n "INFO:" Phase1b report: >> $LOG
eval ${phase1b_cmd} 
err=$?
if [ ${err} != 0 ]
then
    echo "Phase1b run returned error code $err"
    echo "$0 running in cwd=" $(pwd)
    #echo "$0 will continue and do a redo with level=0.98 instead of 0.5"
    echo "$0 will exit.  See cmd above^^."
    exit 1
fi
n=`cat $phase1b_out | wc -l`
echo
echo "First try of Phase1b reported $n frames have objects, see the lines in:"
echo $phase1b_out
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


	phase1b_redo_cmd="$time_cmd_prefix ${SOFTWARE_DIR}/$phase1b "
	#
	# level for Phase1b redo was 0.98 here, now this is set by our DEFAULTS and C-TAP scripts
	#                                                                           --V---
	phase1b_redo_cmd+=" ${RESULTS_DIR}/${RESULT_OF_1a_BASE} $ndiffs ${PHASE_1b_LEVEL_REDO} "
	phase1b_redo_cmd+=" ${OPT_CamSett} "
	phase1b_redo_cmd+="  > $phase1b_out 2>>$LOG"
	( echo  ; echo "${phase1b_redo_cmd}" ) | cat >> ${COMMAND_ARCHIVE_PATHNAME}	
	echo "CMD:"
	$(echofold  "${phase1b_redo_cmd}") >> $LOG
	eval $phase1b_redo_cmd
	err=$?
	phase1b_finish_time=$(uptimenow)
	if [ ${err} != 0 ]
	then
	    echo "phase1b redo run returned error code $err"
	    echo "$0 running in cwd=" $(pwd)
	    #echo "$0 will continue and do a redo with level=0.98 instead of 0.5"
	    echo "$0 will exit.  See cmd above^^."
	    exit 1
	fi

	
	n=`cat $phase1b_out | wc -l`
	echo "phase 1B: Re-Done reports $n frames with objects."
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


if [ $MAKE_BABY_MOVIE"wawa" = "yeswawa" ]
then
    echo "Logging range, deploying Little Green Men..circling 'em in picnn.bmps"
    source "${SOFTWARE_DIR}/code-BABYMOVIE-BMP.h.sh"
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
