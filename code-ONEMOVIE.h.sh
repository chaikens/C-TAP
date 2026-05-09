echo SOURCED: code-ONEMOVIE.h.sh

echo "We got moviePrefix=$moviePrefix"
echo "We got movie_file=$movie_file"

echo "Making Mick West redundant, processing ONI FOIA files..."
FileName="${moviePrefix}.${ext}"
echo "FileName=$FileName"

#
# version 1 logging:
#
#Save as a results the literal commands we gave, so one can
#rerun, debug, etc!
COMMAND_ARCHIVE_PATHNAME=${RESULTS_DIR}/${moviePrefix}.cmds
cat /dev/null > ${COMMAND_ARCHIVE_PATHNAME}    #So we can just append anytime

ALL_MESSAGES_PATH=${RESULTS_DIR}/${moviePrefix}.mess
cat /dev/null > $ALL_MESSAGES_PATH

#
# PipeOpt version logging edited to use v1 vars.
#
#MOVIE_BASE=${MOVIE_FILE%.*} #Basename without the DOT
#should be moviePrefix

#################code for naming and creating the LOG for this run##############
touch "${RESULTS_DIR}/$moviePrefix.log.0" #that one's a dummy, so the next
#numbered one can be computed the first time.

pushd ${RESULTS_DIR}
logn=$( ls $moviePrefix.log.* | sed s/${moviePrefix}.log.// | sort -n | tail -n 1)
popd
((logn++))

LOG=${RESULTS_DIR}/$moviePrefix.log.$logn

cat /dev/null > $LOG #start with an empty log.

#It's a good idea to first commit the scripts, programs, settings, etc to be tested.
#   Of course, the test results will go into the next commit, but at least
#   the state before the experiment is archived.
gitcommit=$(git log HEAD -n 1 | sed -n  's/^\(commit.\{9\}\).*/\1/p')
echo $0 >>$LOG
echo $( date )  >>$LOG
echo $gitcommit  >>$LOG
git remote --verbose  >>$LOG #Worldwide readers of the log can get the software!

xterm -geometry 100x50+1300+0 -sb -title 'C-TAP LOG' -e tail -f $LOG &
xterm_pids+=($!) #for killing 'em
################################################################################




    
echo "Repaying TTSA investors, straightening Uri Gellar's spoons..."
cd $BITMAPS_DIR  #ffmpeg puts bitmaps in its cwd.
    
#ffmpeg -threads 0 -hide_banner -an -i $movie_file -vf "scale=trunc(iw/4)*2:trunc(ih/4)*2,decimate,setpts=N/100/TB" -fps_mode vfr thumb%06d.bmp
    
#
# EXTRACT or REUSE BMPS
#
#
if [ ${REUSE_BMPS}"" != "yes" ]
then
    echo "Extracting .bmp's" >> $LOG
    # BITMAPS_DIR is assurred at the beginning, not sep. for each movie
    rm  -f $BITMAPS_DIR/*
    #aside from saving space, we must delete old bitmaps
    #because old files are not necessarilly written over
    # by the C++ programs.  -f makes succeed even if dir bitmaps doesn't exist.
    # Don't use -r since it's better not remake a dir because
    # we can then watch it from another
    # shell.  (In unix, a dir/file remade with the same name is different.)
	
    #ffmpeg -xerror -threads 0 -hide_banner -an -i ${movie_file}  thumb%06d.bmp  &> ${RESULTS_DIR}/ffmpeg.outputs 

    #we now know decimation is necessary for the FLIR algorithm
    
    movie_width=$(widthOfMovie $movie_file)
    movie_height=$(heightOfMovie $movie_file)
    
    ##scaling? We actually do half-scaling and full scaling.
    #OK diff with brother FullScaling 
    #FFMPEG_EXTRACT_CMD="ffmpeg -xerror -threads 0 -hide_banner -an  \
    #       -i $movie_file                                                 \
    #       -vf                                                            \
    #        'scale=trunc(iw/4)*2:trunc(ih/4)*2,decimate,setpts=N/100/TB'  \
    #       thumb%06d.bmp                                                  \
    #       &> $RESULTS_DIR/ffmpeg.outputs"

    echo Running >> $LOG
    # FFMPEG_EXTRACT_CMD is embedded in ffmpeg_bmp_extract() shell fn from config.
    echo Extracting bmps with >> $LOG
    type ffmpeg_bmp_extract >> $LOG
    echo >> $LOG
    (echo ; type ffmpeg_bmp_extract; echo ) >>${COMMAND_ARCHIVE_PATHNAME}
    
    xterm -geometry 160x30+0+180 -title 'ffmpeg extract bitmaps'  -e tail -f ${RESULTS_DIR}/ffmpeg.outputs &
    Extract_xterm_PID=$!  #so we can kill you later.
	  
    ffmpeg_bmp_extract $movie_file $RESULTS_DIR/ffmpeg.outputs
    
    if [ ! $? ]
    then
	echo
	echo ffmpeg error
	echo opening emacs on ffmpeg output
	emacs ${RESULTS_DIR}/ffmpeg.outputs &
	echo exiting
	exit 1
    fi
    echo 
    echo "ffmpeg done." >> $LOG
    echo
    if ! depthOfBmpIs24 thumb000001.bmp
    then
	echo 'thumb000001.bmp does not exist (in its proper place)'
	echo or its non-24bit depth is not supported by Phase1a
	exit 1
    else
	bmp_width=$(widthOfBmp thumb000001.bmp)
	bmp_height=$(heightOfBmp thumb000001.bmp)
    fi
    #done with extrating bitmaps.

else
    echo
    echo "We're reusing movie bitmaps for debugging speed."
    echo
    echo >> $LOG
    echo "We're reusing movie bitmaps for debugging speed." >> $LOG
    echo >> $LOG
    rm -f pic*.bmp #only delete images used to make the previous "baby movie"
    if ! depthOfBmpIs24 thumb000001.bmp
    then
	echo 'thumb000001.bmp does not exist (in its proper place)'
	echo or its non-24bit depth is not supported by Phase1a
    else
	bmp_width=$(widthOfBmp thumb000001.bmp)
	bmp_height=$(heightOfBmp thumb000001.bmp)
    fi
fi
    
#
# ffmpeg usage explanation for command ABOVE
#
# &> redirects both stdout and stderr to save temporarilly.
# because the FLIRanalysisPhase1aCamX.cpp processes all files in the cwd.
#
# --from ffmpeg documentation:
# -fps_mode vfr (failed with sdc's old 4.4 ffmpeg version, so he removed it)
#  "Frames are passed through with their timestamp or
#   dropped so as to prevent 2 frames from having the same timestamp."
#
# in the filter (quoted string)
#  decimate = "Drop duplicated frames at regular intervals."
#  setpts changes the presentation timestamp (pts)
#    N is the input frame count
#    TB is the Time Base of the input timestamps (usually 1/framerate)
#

nframes=`ls  | wc -l`
echo "$nframes frames captured" >> $LOG

#echo ${moviePrefix}
#This is the input movie name without movie type extension.
#Result files will be named
# ${moviePrefix}.int
# ${moviePrefix}.out
# ${moviePrefix}.MOV (that's the "baby movie")

echo | cat >>$ALL_MESSAGES_PATH
echo "Input Origin Report:" | cat  >> $LOG #$ALL_MESSAGES_PATH
if [ ${movie_width}"" = "" ]
then
    echo 'We are reusing thumb[0-9]^6.bmp-s somehow previously extracted.' >>$LOG #$ALL_MESSAGES_PATH
    echo 'bmp_width='${bmp_width} 'bmp_height='${bmp_height}   >>$LOG #$ALL_MESSAGES_PATH
else
    echo 'We extracted bitmaps from the movie:'  >>$LOG #$ALL_MESSAGES_PATH
    echo $movie_file >>$LOG #$ALL_MESSAGES_PATH
    echo 'movie_width='$movie_width 'movie_height='$movie_height >>$LOG #$ALL_MESSAGES_PATH
    echo 'And, FYI,:' | cat >>$ALL_MESSAGES_PATH
    echo 'bmp_width='$bmp_width 'bmp_height='$bmp_height >>$LOG #$ALL_MESSAGES_PATH
fi

if [ $bmp_width"" != "1920" ]
then
    echo Until a new version is completed, >>$LOG #$ALL_MESSAGES_PATH
    echo clipping is done in Phase1a, maybe other stuff is done in Phase1b, >>$LOG #$ALL_MESSAGES_PATH
    echo based on 1920x1080 bmps, not what you have. >>$LOG #$ALL_MESSAGES_PATH
else
    if [ $bmp_height"" != "1080" ]
    then
	echo Until a new version is completed, >>$LOG #$ALL_MESSAGES_PATH
	echo clipping is done in Phase1a, maybe other stuff is done in Phase1b, >>$LOG #$ALL_MESSAGES_PATH
	echo based on 1920x1080 bmps, not what you have. >>$LOG #$ALL_MESSAGES_PATH
	echo Hmm you have 1920 width, but bmp_height=${bmp_height}  >>$LOG #$ALL_MESSAGES_PATH
    fi
fi

#will put in both phases, though not used in Phase1b yet.
#OK diff with brother --movie-scale 2
scaling_cmd_args="--movie-scale 1 --pixproc-scale 1 "


echo
echo "Beginning Phase1a"
echo

RESULT_OF_1a_BASE="${moviePrefix}.int"
cat /dev/null > ${RESULTS_DIR}/${RESULT_OF_1a_BASE}
#Phase1a used to be will run multiple times, appending each time,
# (but the script now only does one run.)
# If we did staged runs, we must start with nothing.
# Also, this ensures xterm's tail doesn't fail.

xterm -geometry 150x30+0+180 -title 'Phase 1a (.int file) output'  -e tail -f ${RESULTS_DIR}/${RESULT_OF_1a_BASE} -s 0.1 &
Phase1a_xterm_PID=$!  #so we can kill you later.

cd $BITMAPS_PARENT_DIR
#That's where the C++ image processors expect us to be

cat /dev/null > ${RESULTS_DIR}/${RESULT_OF_1a_BASE}
#So stage loop can append results.

if [ true ]
then
    # run Phase1a once on all the frames
    Phase1a_cmd_args="${SOFTWARE_DIR}/$Phase1a 0 $nframes 0"
    Phase1a_cmd_args="${Phase1a_cmd_args} ${opt_phase1a_bitmaps} "
    Phase1a_cmd_args="${Phase1a_cmd_args} ${OPT_CamSett} "
    Phase1a_cmd_args="${Phase1a_cmd_args} ${opt_scaling} "
    Phase1a_cmd_args="${Phase1a_cmd_args} ${OTHER_OPTIONS} "
    Phase1a_cmd="${Phase1a_cmd_args} >> ${RESULTS_DIR}/${RESULT_OF_1a_BASE} 2>>$LOG #$ALL_MESSAGES_PATH"

    echo Running
    echo ${Phase1a_cmd}
    ( echo ; echo ${Phase1a_cmd} ) | cat >>${COMMAND_ARCHIVE_PATHNAME}
    
    eval ${Phase1a_cmd} 
    err=$?
    if [ ${err} != 0 ]
    then
	echo "Phase1a run in a single stage returned error code $err"
	echo "$0 running in cwd=" $(pwd)
	echo "$0 will exit. Heres the bad command:"
	echo
	echo ${Phase1a_cmd}
	echo
	exit 1
    fi

    
    #first 0 = first frame number
    #2nd 0 = CC (Cloud Cover) parameter; not used.
else
    #
    # orig. code runs Phase1 in stages
    # we must consult Matt Sz. to see if we should get rid of this.
    # One frame pair comparison is lost for each new stage.
    # 
    u=0; v=0
    w=30
    t=$((f/w))
    echo "Removing any quantum woo; reticulating $t splines..."
    #since the stager appends to the result file..
    cat /dev/null > ${RESULTS_DIR}/${RESULT_OF_1a_BASE}
    while [ $u -lt $nframes ]
    do
	
	Phase1a_cmd_args="${SOFTWARE_DIR}/$Phase1a $u $t 0 "
	#OK diff with brother FullScaling
	Phase1a_cmd_args="${Phase1a_cmd_args} ${opt_phase1a_bitmaps} "
	Phase1a_cmd_args="${Phase1a_cmd_args} ${OPT_CamSett} "
	Phase1a_cmd_args="${Phase1a_cmd_args} ${opt_scaling} "
	
	Phase1a_cmd_args="${Phase1a_cmd_args} ${OTHER_OPTIONS} "
	Phase1a_cmd="${Phase1a_cmd_args} >> ${RESULTS_DIR}/${RESULT_OF_1a_BASE} 2>>$LOG #$ALL_MESSAGES_PATH"

	eval ${Phase1a_cmd}  #this does the trick
	err=$?
	if [ ${err} != 0 ]
	then
	    echo "Phase1a in loop of stages returned error code $err"
	    echo "$0 running in cwd=" $(pwd)
	    echo "$0 will exit. Heres the bad command:"
	    echo
	    echo ${Phase1a_cmd}
	fi
	
	((++v))
	echo Phase 1A: From $u to $((t+u)) "," Part $v of $w Done
	if [ $v -gt $w ]
	then
	    echo "Don't panic -- there was a remainder from the division!"
	fi
	u=$((u+t))
	if [ $((u+t)) -gt $nframes ]
	then
	    t=$((nframes-u))
	    fi
    done
fi

ndiffs=$(cat ${RESULTS_DIR}/${RESULT_OF_1a_BASE} | wc -l )
echo
echo Phase1a computed $(cat ${RESULTS_DIR}/${RESULT_OF_1a_BASE} | wc -l ) difference lines "in"
echo "${RESULTS_DIR}/${RESULT_OF_1a_BASE}"

echo | cat >>$LOG #ALL_MESSAGES_PATH
echo Phase1a computed $(cat ${RESULTS_DIR}/${RESULT_OF_1a_BASE} | wc -l ) difference lines "in" >>$LOG #$ALL_MESSAGES_PATH
echo "${RESULTS_DIR}/${RESULT_OF_1a_BASE}"  >>$LOG #$ALL_MESSAGES_PATH

#Someday result files might include comments, so simple wc won't work to get the number of data lines.
    
echo
echo Beginning Phase1b
echo
    
RESULT_OF_1b_BASE="${moviePrefix}.out"
cat /dev/null > ${RESULTS_DIR}/${RESULT_OF_1b_BASE}
    
#so xterm's less doesnt fess. We use less since the result is finished fast.
xterm -geometry 80x80+0+0 -sb -title 'Phase1b .out' -e less -f ${RESULTS_DIR}/${RESULT_OF_1b_BASE} &
Phase1b_xterm_PID=$!

#
# level for Phase1b first try is 0.5, here----------------------------------------------V---
#
Phase1b_cmd_args="${SOFTWARE_DIR}/$Phase1b ${RESULTS_DIR}/${RESULT_OF_1a_BASE} $ndiffs 0.5 "
Phase1b_cmd_args="${Phase1b_cmd_args} ${OPT_CamSett} $scaling_cmd_args"
Phase1b_cmd="${Phase1b_cmd_args} > ${RESULTS_DIR}/${RESULT_OF_1b_BASE} 2>>$LOG"

echo Running
echo ${Phase1b_cmd} 
( echo ; echo ${Phase1b_cmd} ) | cat >>${COMMAND_ARCHIVE_PATHNAME}
#HUH? commanding ${Phase1b_cmd} makes some shell fail to redirect stdout!
#${Phase1b_cmd_args} > ${RESULT_DIR}/${RESULT_OF_1b_BASE}

echo "First 1b try:" | cat >> $LOG #

xterm -geometry 180x30+50+180 -title 'Phase 1b messages'  -e less -f $MESS_1b_FULL_PATH &
Phase1b_mess_xterm_PID=$!  #so we can kill you later.

echo ${Phase1b_cmd} | cat >> $LOG #ALL_MESSAGES_PATH
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
	Phase1b_redo_command_args="${SOFTWARE_DIR}/$Phase1b ${RESULTS_DIR}/${RESULT_OF_1a_BASE} $ndiffs 0.98 "
	Phase1b_redo_command_args="${Phase1b_redo_command} ${OPT_CamSett} $scaling_cmd_args"
	Phase1b_redo_command="${Phase1b_redo_command_args} > ${RESULTS_DIR}/${RESULT_OF_1b_BASE} 2>> $LOG"
	( echo  ; echo "${Phase1b_redo_command}" ) | cat >> ${COMMAND_ARCHIVE_PATHNAME}
	echo "${Phase1b_redo_command}" | cat >> $MESS_1b_FULL_PATH
	eval $Phase1b_redo_command
	err=$?
	if [ ${err} != 0]
	then
	    echo "Phase1b redo run returned error code $err"
	    echo "$0 running in cwd=" $(pwd)
	    #echo "$0 will continue and do a redo with level=0.98 instead of 0.5"
	    echo "$0 will exit.  See cmd above^^."
	    exit 1
	fi

	
	n=`cat ${RESULTS_DIR}/${RESULT_OF_1b_BASE} | wc -l`
	echo "Phase 1B: Re-Done reports $n frames with objects."
	
    fi
fi
    
#now, after either one or a redo of Phase1b,
# don't waste time on empty file OR overloading your HD
if [ $n -gt 465000 ] || [ $n -eq 0 ]
then
    echo "Either too many or too few objects..."
    echo "On to the next movie, if any"
    continue #top level movie loop
fi
# number above assumes 2.9 MB/image: (2.9*(46500+216e3))/1e3 = 760 GB of free space needed! Adjust for your machine


echo "Logging range, deploying Little Green Men..circling 'em in picnn.bmps"
source "${SOFTWARE_DIR}/code-BMP-BABYMOVIE.h.sh"

# We're done with I/O thru the bitmaps dir
cd ${SOFTWARE_DIR}

# save the result movie
    
    
#echo "Cleaning detritus..." #I'd rather leave that for the build system.

#rm -rf *.cpp~ *.sh~ *.txt~ *.err bitmaps/
# sdc: Keep old bitmaps BUT the script removes them early next time
# because the C++ programs process all files in their cwd.

#rm -f *.cpp~ *.sh~ *.txt~ 

if [ $COPYMOVIES ]
then
    echo "about to delete input movie"
    echo $movie_file
    rm $movie_file
fi
    
echo "Cosmic consciousness has been achieved!!"
echo "from yet another movie $o of $numMovieFiles."

