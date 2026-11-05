#echo SOURCED: code-PIPELINE-EXTRACT-1A.h.sh

#pushd $PIPE_DIR > /dev/null


if [ ! -r ${movie_file} ];then echo Cant read movie file; exit 1; fi


movie_base=${movie_file%.*} #filename (absolute or relative) without the DOT
result_of_1a=$movie_base.int #NOT the name of the pipe, that's PIPE.int

#echo "CMD:" "The frame extraction command is" | cat >> $LOG
#(echo "CMD:"; type ffmpeg_pipe_extract) | cat >> $LOG
#(type ffmpeg_pipe_extract; echo -n "cd "; pwd; ) >>${COMMAND_ARCHIVE_PATHNAME}
#dandy shell way to set a value to a null or undef. param --------V--
echo "INFO:" "Frame resolution is Movie resolution / ${MOVIE_TO_FRAME_DIV:=1}" | cat >> $LOG

#echo ${FFMPEG_EXTRACT_FILTER}  >>${COMMAND_ARCHIVE_PATHNAME}

# shell function defined by the script that included me
 width=$(($(widthOfMovie  ${movie_file})/MOVIE_TO_FRAME_DIV))
height=$(($(heightOfMovie ${movie_file})/MOVIE_TO_FRAME_DIV))

yuvsizeb=$(( (width*height*3)/2 ))

echo "INFO:" Extracted frames width=${width} height=${height} yuvsizeb=${yuvsizeb} | cat >> $LOG

echo "INFO:" Processing ${width}x${height} frames from $movie_file | cat >> $LOG

yuvpipe=${PIPE_DIR}/${JOBNAME}PIPE.yuv
rm -f $yuvpipe
mknod $yuvpipe p

bmppipe=${PIPE_DIR}/${JOBNAME}PIPE.bmp
rm -f ${bmppipe}
mknod ${bmppipe} p

now=$(uptimenow)
echo "STEP:" 'STARTING pipeline EXTRACTION->YUVtoBMP->Phase1a at ' ${now} "seconds." >> $LOG
echo 'STEP: Starting pipeline EXTRACTION->YUVtoBMP->Phase1a' Please wait...
date

if [ ${xterm_ffmpeg_pid}x = "x" ]
then
    touch ${RESULTS_DIR}/ffmpeg.log
    xterm -bg '#E0FFFF' ${XTERM_PARAM} -geometry 160x30+0+100 -title 'ffmpeg extract bitmaps'  -e tail -f ${RESULTS_DIR}/ffmpeg.log &
    #xterm -geometry 160x30+0+100 -title 'ffmpeg extract bitmaps'  -e less -f +F ${RESULTS_DIR}/ffmpeg.log &
    xterm_ffmpeg_pid=$!  #to ensure just one xterm for ffmpeg; may kill at end.
fi

$(ffmpeg_pipe_extract ${movie_file}) & #MUST BE IN BACKGROUND!!
#${JOBNAME}PIPE.yuv is hardcoded in ffmpeg_pipe_extract.

#Will experiment with more parameters.  The original C-TAP
# cut back the resolution.

# should be symbolically linked from top C-TAP dir
# Soon we'll experiment more with this filter, maybe better config way.
# pipe filenames must end in .yuv and .bmp respectively 
yuv_filter_cmd="${SOFTWARE_DIR}/YUVToBMPStreamFilter   $width $height < ${yuvpipe} > ${bmppipe}  2>>$LOG"
( echo "CMD:"; echo $yuv_filter_cmd ; echo )  >>$LOG
( echo ; echo -n "cd "; pwd;  echo $yuv_filter_cmd ; echo )  >>${COMMAND_ARCHIVE_PATHNAME}
# width and height are necessary since yuv frames are raw.
# so we have to take care if ffmpeg outputs differently sized
# frames from the movie original.
eval $time_cmd_prefix $yuv_filter_cmd &

echo "TIME:" Started YUVToBMPStreamFilter at /proc/uptime=$(uptimenow) seconds. | cat >> $LOG

echo "STEP:" $0 "STARTING Phase1aPipeOpt"  | cat >> $LOG
phase1a_start_time=$(uptimenow)
prt=" /proc/uptime=${phase1a_start_time}"
echo "TIME:" $prt >> $LOG

phase1a_cmd_args="$time_cmd_prefix ${SOFTWARE_DIR}/${PHASE_1a} --pipeline "
phase1a_cmd_args+=" ${phase1a_options} "
phase1a_cmd_args+=" 0 100000000 0 "
phase1a_cmd_args+=" --write-mask-file ${RESULTS_DIR}/${IMAGE_MASK_BASE}"
phase1a_cmd_args+=" < ${bmppipe} "
phase1a_cmd="${phase1a_cmd_args} >> ${RESULTS_DIR}/${RESULT_OF_1a_BASE} 2>>$LOG" 

(echo "CMD:" ; echo "CMD:" ${phase1a_cmd}; echo ) >> $LOG
(echo ; echo -n "cd "; pwd; echo ${phase1a_cmd}; echo ) >> ${COMMAND_ARCHIVE_PATHNAME}
#echo "About to call Phase1a  ...Report is in Log."
#echo "eval" "${phase1a_cmd}"

eval " ${phase1a_cmd}"  #FOREGROUND. stderr from cmd and time go to LOG.
RET=$?
echo >>$LOG #space after time report

phase1a_finish_time=$(uptimenow)
#echo Finished: $phase1a_finish_time
echo "TIME:" Finished: $phase1a_finish_time >> $LOG
phase1a_net_time=$(numdif $phase1a_finish_time $phase1a_start_time)

echo "TIME:" "${PHASE_1a} FINISHED returned $RET, took wallclock time ${phase1a_net_time} sec." >> $LOG
echo INFO: "${PHASE_1a} FINISHED returned $RET, took wallclock time ${phase1a_net_time} sec."
#date

echo "INFO:" "Find Phase1aPipeOpt results in  ${RESULTS_DIR}/${RESULT_OF_1a_BASE}" >> $LOG

rm -f ${bmppipe}
rm -f ${yuvpipe}

if [ $RET != 0 ]
then
    echo ERR: $phase1a_cmd
    echo returned status $RET
    echo $0 exiting, bye bye.
    exit $RET
fi
