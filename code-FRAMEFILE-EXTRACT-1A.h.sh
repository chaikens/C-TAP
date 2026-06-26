echo SOURCED: code-FRAMEFILE-EXTRACT-1A.h.sh
echo "bitmaps dir="${BITMAPS_DIR}

pushd $BITMAPS_DIR  > /dev/null #ffmpeg puts bitmaps in its cwd.
    
#ffmpeg -threads 0 -hide_banner -an -i $movie_file -vf "scale=trunc(iw/4)*2:trunc(ih/4)*2,decimate,setpts=N/100/TB" -fps_mode vfr thumb%06d.bmp
    
#
# EXTRACT or REUSE BMPS
#
#
must_extract=yes
if [ ${REUSE_BMPS}"" = "yes" ]
then
    if ! depthOfBmpIs24 thumb000001.bmp
    then
	echo 'thumb000001.bmp does not exist (in its proper place)'
	echo "or its non-24bit depth is not supported by Phase1a"
	echo "We will NOT reuse bitmaps, sorry"
	echo 'INFO: thumb000001.bmp does not exist (in its proper place)' >> $LOG
	echo "INFO: or its non-24bit depth is not supported by Phase1a"   >> $LOG
	echo "INFO: We will NOT reuse bitmaps, sorry"                     >> $LOG
    else
	bmp_width=$(widthOfBmp thumb000001.bmp)
	bmp_height=$(heightOfBmp thumb000001.bmp)
	must_extract=no
	echo
	echo "We're reusing movie bitmaps for debugging speed."
	echo "INFO:" >> $LOG
	echo "INFO:" "We're reusing movie bitmaps for debugging speed." >> $LOG
	echo >> $LOG
	(echo ; echo "#Reused bitmaps, so no extract commands."  ) >>${COMMAND_ARCHIVE_PATHNAME}
	#move to where baby movie is made 
	#rm -f pic*.bmp #only delete images used to make the previous "baby movie"
    fi
fi

if [ ${must_extract} = yes ]
then
    echo "STEP:" "Extracting .bmp's" >> $LOG
    echo "Extracting .bmp's"
    
    # BITMAPS_DIR is assurred at the beginning, not sep. for each movie
    rm  -f $BITMAPS_DIR/*
    #aside from saving space, we must delete old bitmaps
    #because old files are not necessarilly written over
    # by the C++ programs.  -f makes succeed even if dir bitmaps doesn't exist.
    # Don't use -r since it's better not remake a dir because
    # we can then watch it from another
    # shell.  (In unix, a dir/file remade with the same name is different.)
	
    #ffmpeg -xerror -threads 0 -hide_banner -an -i ${movie_file}  thumb%06d.bmp  &> ${RESULTS_DIR}/ffmpeg.log 

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
    #       &> $RESULTS_DIR/ffmpeg.log"

    echo "STEP:" Running >> $LOG
    # FFMPEG_EXTRACT_CMD is embedded in ffmpeg_bmp_extract() shell fn from config.
    echo "PGM:" Extracting bmps with >> $LOG
    type ffmpeg_bmp_extract >> $LOG
    echo "PGM:" ffmpeg filter is "${FFMPEG_EXTRACT_FILTER}" >> $LOG
    echo "PGM:" >> $LOG
    (echo ; type ffmpeg_bmp_extract; echo ) >>${COMMAND_ARCHIVE_PATHNAME}
    echo "FFMPEG_EXTRACT_FILTER=${FFMPEG_EXTRACT_FILTER}" >>${COMMAND_ARCHIVE_PATHNAME}

    if [ ${xterm_ffmpeg_pid}x = "x" ]
    then
	touch ${RESULTS_DIR}/ffmpeg.log
	xterm -bg '#E0FFFF' ${XTERM_PARAM} -geometry 160x30+0+100 -title 'ffmpeg extract bitmaps'  -e tail -f ${RESULTS_DIR}/ffmpeg.log &
	#xterm -geometry 160x30+0+100 -title 'ffmpeg extract bitmaps'  -e less -f +F ${RESULTS_DIR}/ffmpeg.log &
	xterm_ffmpeg_pid=$!  #to ensure just one xterm for ffmpeg; may kill at end.
    fi

    extract_start_time=$(uptimenow)
    echo "ffmpeg is extracting frames into many many .bmp files"
    echo "stated at $extract_start_time sec."
    echo "See progress in the ffmpeg extract bitmaps window. Now's a good coffee break time."
    echo "TIME:" "ffmpeg extraction started at $extract_start_time sec." >> $LOG

    ffmpeg_bmp_extract $movie_file $RESULTS_DIR/ffmpeg.log
    ret=$?

    extract_finish_time=$(uptimenow)
    echo "extraction done at ${extract_finish_time} sec."
    et=$(numdif $extract_finish_time $extract_start_time)
    echo "That took $et seconds."
    echo "TIME: extraction done at $extract_finish_time, took $et sec." >> $LOG
    
    if [ ! $ret ]
    then
	echo
	echo ffmpeg error
	echo opening emacs on ffmpeg output
	emacs ${RESULTS_DIR}/ffmpeg.log &
	echo exiting
	exit 1
    fi
    echo 
    echo "STEP:ffmpeg done." >> $LOG
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
# in the filter (quoted string) there are TWO different filters:
#  decimate = "Drop duplicated frames at regular intervals."
#  setpts changes the presentation timestamp (pts)
#    N is the input frame count
#    TB is the Time Base of the input timestamps (usually 1/framerate)
#

nframes=`ls thumb*.bmp | wc -w`
echo "INFO:" "$nframes frames captured" >> $LOG

#echo ${moviePrefix}
#This is the input movie name without movie type extension.
#Result files will be named
# ${moviePrefix}.int
# ${moviePrefix}.out
# ${moviePrefix}.MOV (that's the "baby movie")

echo "INFO:" | cat >>$LOG
echo "INFO: Input Origin Report:" | cat  >> $LOG 
if [ ${REUSE_BMPS} = "yes" ]
then
    echo "INFO:" 'We are reusing thumb[0-9]^6.bmp-s somehow previously extracted.' >>$LOG 
    echo "INFO:" 'bmp_width='${bmp_width} 'bmp_height='${bmp_height}   >>$LOG 
else
    echo "INFO:" 'We extracted bitmaps from the movie:'  >>$LOG 
    echo "INFO:" $movie_file >>$LOG 
    echo "INFO:" 'movie_width='$movie_width 'movie_height='$movie_height >>$LOG 
    echo "INFO:" 'And, FYI,:' | cat >>${LOG}
    echo "INFO:" 'bmp_width='$bmp_width 'bmp_height='$bmp_height >>$LOG
fi

if [ $bmp_width"" != "1920" ]
then
    echo "DEVEL:" Until a new version is completed, >>$LOG 
    echo "DEVEL:" clipping is done in Phase1a, maybe other stuff is done in Phase1b, >>$LOG 
    echo "DEVEL:" based on 1920x1080 bmps, not what you have. >>$LOG
else
    if [ $bmp_height"" != "1080" ]
    then
	echo "DEVEL:" Until a new version is completed, >>$LOG 
	echo "DEVEL:" clipping is done in Phase1a, maybe other stuff is done in Phase1b, >>$LOG 
	echo "DEVEL:" based on 1920x1080 bmps, not what you have. >>$LOG 
	echo "DEVEL:" Hmm you have 1920 width, but bmp_height=${bmp_height}  >>$LOG 
    fi
fi

echo
echo $0 "STARTING Phase1a"
echo
#if [ true ]
#then

# run Phase1a once on all the frames
phase1a_cmd_args="$time_cmd_prefix ${SOFTWARE_DIR}/${PHASE_1a}"
phase1a_cmd_args+=" ${phase1a_options} "
phase1a_cmd_args+=" --write-mask-file ${RESULTS_DIR}/${IMAGE_MASK_BASE}"
phase1a_cmd="${phase1a_cmd_args} 0 $nframes 0 >> ${RESULTS_DIR}/${RESULT_OF_1a_BASE} 2>>$LOG" 
  
echo Running
echofold ${phase1a_cmd}
( echo ; echo ${phase1a_cmd} )  >>${COMMAND_ARCHIVE_PATHNAME}
echo -n "PGM: " >>${LOG}
echofold ${phase1a_cmd}  >>${LOG}

eval ${phase1a_cmd} 
err=$?
if [ ${err} != 0 ]
then
    echo "Phase1a run in a single stage returned error code $err"
    echo "$0 running in cwd=" $(pwd)
    echo "$0 will exit. Heres the bad command:"
    echo
    echo ${phase1a_cmd}
    echo
    exit 1
fi

    
    #first 0 = first frame number
    #2nd 0 = CC (Cloud Cover) parameter; not used.
#else
    #
    # orig. code runs Phase1 in stages
    # we must consult Matt Sz. to see if we should get rid of this.
    # One frame pair comparison is lost for each new stage.
    # 
    #u=0; v=0
    #w=30
    #t=$((f/w))
    #echo "Removing any quantum woo; reticulating $t splines..."
    #since the stager appends to the result file..
    #cat /dev/null > ${RESULTS_DIR}/${RESULT_OF_1a_BASE}
    #while [ $u -lt $nframes ]
    #do
	
	#Phase1a_cmd_args="${SOFTWARE_DIR}/$Phase1a $u $t 0 "
	#OK diff with brother FullScaling
	#Phase1a_cmd_args="${Phase1a_cmd_args} ${opt_phase1a_bitmaps} "
	#Phase1a_cmd_args="${Phase1a_cmd_args} ${OPT_CamSett} "
	#Phase1a_cmd_args="${Phase1a_cmd_args} ${opt_scaling} "
	
	#Phase1a_cmd_args="${Phase1a_cmd_args} ${OTHER_OPTIONS} "
	#Phase1a_cmd_args="${Phase1a_cmd_args} ${PHASE1A_OTHER_OPTIONS} "
	#Phase1a_cmd="${Phase1a_cmd_args} >> ${RESULTS_DIR}/${RESULT_OF_1a_BASE} 2>>$LOG" 

	#eval ${Phase1a_cmd}  #this does the trick
	#err=$?
	#if [ ${err} != 0 ]
	#then
	    #echo "Phase1a in loop of stages returned error code $err"
	    #echo "$0 running in cwd=" $(pwd)
	    #echo "$0 will exit. Heres the bad command:"
	    #echo
	    #echo ${Phase1a_cmd}
	#fi
	
	#((++v))
	#echo Phase 1A: From $u to $((t+u)) "," Part $v of $w Done
	#if [ $v -gt $w ]
	#then
	 #   echo "Don't panic -- there was a remainder from the division!"
	#fi
	#u=$((u+t))
	#if [ $((u+t)) -gt $nframes ]
	#then
	 #   t=$((nframes-u))
	#fi
    #done
#fi

ndiffs=$(cat ${RESULTS_DIR}/${RESULT_OF_1a_BASE} | wc -l )
echo
echo Phase1a computed $(cat ${RESULTS_DIR}/${RESULT_OF_1a_BASE} | wc -l ) difference lines "in"
echo "${RESULTS_DIR}/${RESULT_OF_1a_BASE}"

echo "INFO:" >>$LOG 
echo "INFO:" Phase1a computed $(cat ${RESULTS_DIR}/${RESULT_OF_1a_BASE} | wc -l ) difference lines "in" >>$LOG 
echo "INFO:" "${RESULTS_DIR}/${RESULT_OF_1a_BASE}"  >>$LOG 

popd > /dev/null

#Someday result files might include comments, so simple wc won't work to get the number of data lines.
