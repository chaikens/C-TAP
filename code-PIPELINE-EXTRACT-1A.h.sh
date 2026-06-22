echo SOURCED: code-PIPELINE-EXTRACT-1A.h.sh

pushd $PIPE_DIR > /dev/null

################################# from runthis.halfdecimated.sh #######

#example moviePrefix = DroneShortHalfDecimated
#  movie_file=fullpathname

MOVIE_FILE=DS1HalfDecimatedYUVTablePipe.mp4
MOVIE_BASE=${MOVIE_FILE%.*} #Basename without the DOT
# We have $LOG
result_file_1a=${RESULTS_DIR}/${RESULT_OF_1a_BASE}

###---working ^^^^
#maybe restore timing later  Script below is GONE.
#########   Code what to do here to go in our  time command ########################
#                                                                                  #
#  DO_ONE_MOVIE_SHELL_SCRIPT=./DoOneMovieHalfDecimated1ab.sh                       #
#                                                                                  #
####################################################################################
#/usr/bin/time -o ${MOVIE_BASE}.time \
#     -f 'real    %U\nuser  %E\nsys      %S' \
#     ${DO_ONE_MOVIE_SHELL_SCRIPT} \
#     $MOVIE_FILE $LOG &
####################################################################################

#wait %/usr/bin/time

#echo 
#echo Time Report
#cat ${MOVIE_BASE}.time
###############################################


########################### from DoOneMovieHalfDecimated1ab.sh ###
#### we only do 1a here #########################################
#!/usr/bin/bash
#/*
# DoOneMovieHalfDecimated1ab.sh MOVIEFILENAME.mp4 Logfile
#
# Half resolution and decimation coded by ffmpeg filter below.  NO--parameters do it.
#
# All files should be in or symbolically linked
# in the CWD.  NO MORE!!
#
# Result: Phase1a report, one line for each frame,
# named MOVIEFILENAME.int
# You can watch progress with tail -f MOVIEFILENAME.int
#
# Required programs:
# ffmpeg
# ./YUVToBMPStreamFilter
# ./Phase1a (or other name for variations)
# ./Phase1bPipeOpt
#*/



###################  from DoOneMoviePipeCommon.h.sh


# (2) scale settings--depend on movie and subsequent processing.
#     Here defaults are documented
#
#     If ffmpeg_pipe_extract changes the resolution then this
#     parameter must be set.  For example, with the 2nd extracting
#     function, you must code MOVIE_TO_FRAME_DIV=2
#
#     MOVIE_TO_FRAME_DIV (undefined, defaults to 1)
#
#     MOVIE_SCALE_OPTION="--movie-scale 1"
#     PIXPROC_SCALE_OPTION="--pixproc-scale 1"
#     USER_SCALE_OPTION="--user-scale 1"
#     OTHER_OPTIONS (undefined, so it expands to ""
# After this regression test against orig stuff, movie scale (relative
# to the others) should be read from the original .mp4 movie, using
# widthOfMovie() and heightOfMovie() below. (eventually put in HELPERS).

#
#
# All files should be in or symbolically linked
# in the CWD.  NO MORE
#
# Result: Phase1a report, one line for each frame,
# named MOVIEFILENAME.int
# You can watch progress with tail -f MOVIEFILENAME.int


############################
# STILL RELEVENT
# Required programs:
# ffmpeg
# ./YUVToBMPStreamFilter
# ./Phase1aPipeOpt (or other name for variations)
# ./Phase1bPipeOpt
#*/

if [ ! -r ${movie_file} ];then echo Cant read movie file; exit 1; fi


movie_base=${movie_file%.*} #filename (absolute or relative) without the DOT
result_of_1a=$movie_base.int #NOT the name of the pipe, that's PIPE.int

echo "CMD:" "The frame extraction command is" | cat >> $LOG
(echo "CMD:"; type ffmpeg_pipe_extract) | cat >> $LOG
type ffmpeg_pipe_extract >>${COMMAND_ARCHIVE_PATHNAME}
#dandy shell way to set a value to a null or undef. param --------V--
echo "INFO:" "Frame resolution is Movie resolution / ${MOVIE_TO_FRAME_DIV:=1}" | cat >> $LOG

echo ${FFMPEG_EXTRACT_FILTER}  >>${COMMAND_ARCHIVE_PATHNAME}

# shell function defined by the script that included me
 width=$(($(widthOfMovie  ${movie_file})/MOVIE_TO_FRAME_DIV))
height=$(($(heightOfMovie ${movie_file})/MOVIE_TO_FRAME_DIV))

yuvsizeb=$(( (width*height*3)/2 ))

echo "INFO:" Extracted frames width=${width} height=${height} yuvsizeb=${yuvsizeb} | cat >> $LOG

echo "INFO:" Processing ${width}x${height} frames from $movie_file | cat >> $LOG

yuvpipe=${PIPE_DIR}/PIPE.yuv
rm -f $yuvpipe
mknod $yuvpipe p

bmppipe=${PIPE_DIR}/PIPE.bmp
rm -f ${bmppipe}
mknod ${bmppipe} p

now=$(uptimenow)
echo "STEP:" 'STARTING pipeline EXTRACTION->YUVtoBMP->Phase1a at ' ${now} "seconds." >> $LOG
echo 'STARTING pipeline EXTRACTION->YUVtoBMP->Phase1a at ' $now "seconds."


$(ffmpeg_pipe_extract ${movie_file}) & #MUST BE IN BACKGROUND!!
#PIPE.yuv is hardcoded in ffmpeg_pipe_extract.

#Will experiment with more parameters.  The original C-TAP
# cut back the resolution.

# should be symbolically linked from top C-TAP dir
# Soon we'll experiment more with this filter, maybe better config way.
# pipe filenames must end in .yuv and .bmp respectively 
yuv_filter_cmd="${SOFTWARE_DIR}/YUVToBMPStreamFilter   $width $height < ${yuvpipe} > ${bmppipe}  2>>$LOG"
( echo "CMD:"; echo $yuv_filter_cmd ; echo )  >>$LOG
( echo ; echo $yuv_filter_cmd ; echo )  >>${COMMAND_ARCHIVE_PATHNAME}
# width and height are necessary since yuv frames are raw.
# so we have to take care if ffmpeg outputs differently sized
# frames from the movie original.
eval $time_cmd_prefix $yuv_filter_cmd &

echo "TIME:" Started YUVToBMPStreamFilter at /proc/uptime=$(uptimenow) seconds. | cat >> $LOG

echo
echo $0 "Begnning Phase1a"
echo

echo "STEP:" $0 "STARTING Phase1aPipeOpt"  | cat >> $LOG
phase1a_start_time=$(uptimenow)
prt=" /proc/uptime=${phase1a_start_time} sec., please wait; See progress in Phasa1a .int window."
echo $prt
echo "TIME:" $prt >> $LOG

phase1a_cmd_args="$time_cmd_prefix ${SOFTWARE_DIR}/${PHASE_1a} --pipeline "
phase1a_cmd_args+=" ${phase1a_options} "
phase1a_cmd_args+=" 0 100000000 0 "
phase1a_options+=" --write-mask-file ${RESULTS_DIR}/${IMAGE_MASK_BASE}"
phase1a_cmd_args+=" < ${bmppipe} "
phase1a_cmd="${phase1a_cmd_args} >> ${RESULTS_DIR}/${RESULT_OF_1a_BASE} 2>>$LOG" 

(echo "CMD:" ; echo "CMD:" ${phase1a_cmd}; echo ) >> $LOG
(echo ; echo ${phase1a_cmd}; echo ) >> ${COMMAND_ARCHIVE_PATHNAME}
echo "About to call Phase1a from within /usr/bin/time --verbose ...Report is in Log."
echo "eval" "/usr/bin/time ${phase1a_cmd}"

eval " ${phase1a_cmd}"  #FOREGROUND. stderr from cmd and time go to LOG.
RET=$?
echo >>$LOG #space after time report

phase1a_finish_time=$(uptimenow)
echo Finished: $phase1a_finish_time
echo "TIME:" Finishted: $phase1a_finish_time >> $LOG
phase1a_net_time=$(numdif $phase1a_finish_time $phase1a_start_time)

echo "TIME:" "$0 FINISHED returned $RET, took wallclock time ${phase1a_net_time} sec." >> $LOG
echo "$0 FINISHED returned $RET, took wallclock time ${phase1a_net_time} sec." 

echo "INFO:" "Find Phase1aPipeOpt results in  ${RESULTS_DIR}/${RESULT_OF_1a_BASE}" >> $LOG

if [ $RET != 0 ]
then
    echo $phase1a_cmd
    echo returned status $RET
    echo $0 exiting, bye bye.
    exit $RET
fi

