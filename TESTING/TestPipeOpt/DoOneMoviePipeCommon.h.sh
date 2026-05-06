
#/*
# DoOneMoviePipeCommon.h.sh 
# MUST NOT BE EXECUTABLE
# WILL BE SOURCED BY SCRIPTS
# THAT DEFINE (1) and (2)
# (1) extraction pipeline
# function ffmpeg_pipe_extract()
# DEFINED LIKE
#
#     {
#       ffmpeg -hide_banner -y -i $1 PIPE.yuv &> ffmpeg.log
#      }
# or
#
#   {                                      \
#       ffmpeg  -hide_banner -y -an        \
#       -i $1                               \
#        -vf                                \
#        "scale=trunc(iw/4)*2:trunc(ih/4)*2,decimate,setpts=N/100/TB" \
#	PIPE.yuv &> ffmpeg.log              \
#   }
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



if [ $(type -t ffmpeg_pipe_extract) != "function" ] 
then
    echo $0: ffmpeg_pipe_extract must be defined as a 
    echo shell function before sourcing DoOneMoviePipeCommon.h.sh
    exit 1
fi


#
#
# All files should be in or symbolically linked
# in the CWD.
#
# Result: Phase1a report, one line for each frame,
# named MOVIEFILENAME.int
# You can watch progress with tail -f MOVIEFILENAME.int
#
# Required programs:
# ffmpeg
# ./YUVToBMPStreamFilter
# ./Phase1aPipeOpt
# ./Phase1bPipeOpt
#*/

LOG=$2

#one of very few messages directly to user.
echo Running $0.
echo All echos go to
echo '   ' $LOG
echo except for exiting errors.

echo Running $0 $1 $2 | cat >> $LOG

#no more.V
#echo This $0 exits after the first try of Phase1bPipe | cat >>$LOG

function widthOfMovie() {
    ffprobe -v quiet  -show_streams $@ | sed -n 's/^width=\([1-9][0-9]*\)$/\1/p'
}

function heightOfMovie() {
    ffprobe -v quiet  -show_streams $@ | sed -n 's/^height=\([1-9][0-9]*\)$/\1/p'
    }



MOVIE_FILE=$1
if [ ! -r ${MOVIE_FILE} ];then echo Cant read movie file; exit 1; fi


#Future..set this via configuration file of variable setting commands.
SOFTWARE_DIR=$(pwd)
RESULTS_DIR=$(pwd)
movie_base=${MOVIE_FILE%.*} #filename (absolute or relative) without the DOT
result_of_1a=$movie_base.int #NOT the name of the pipe, that's PIPE.int
result_of_1b_first_try=$movie_base.out #No piping from that yet, will be PIPE.out
result_of_1b_second_try=$movie_base.redo.out

echo "The frame extraction command is" | cat >> $LOG
type ffmpeg_pipe_extract | cat >> $LOG

#dandy shell way to set a value to a null or undef. param --------V--
echo "Frame resolution is Movie resolution / ${MOVIE_TO_FRAME_DIV:=1}" | cat >> $LOG


# shell function defined by the script that included me
 width=$(($(widthOfMovie  ${MOVIE_FILE})/MOVIE_TO_FRAME_DIV))
height=$(($(heightOfMovie ${MOVIE_FILE})/MOVIE_TO_FRAME_DIV))

yuvsizeb=$(( (width*height*3)/2 ))

echo Extracted frames width=${width} height=${height} yuvsizeb=${yuvsizeb} | cat >> $LOG

echo Processing ${width}x${height} frames from $MOVIE_FILE | cat >> $LOG

rm -f PIPE.yuv
mknod PIPE.yuv p

rm -f PIPE.bmp
mknod PIPE.bmp p

rm -f PIPE.int
mknod PIPE.int p

# shell function defined by the script that included me
$(ffmpeg_pipe_extract ${MOVIE_FILE} ) &

#Will experiment with more parameters.  The original C-TAP
# cut back the resolution.
echo STARTED ffmpeg at /proc/uptime= | cat >> $LOG
echo $( cat /proc/uptime ) | sed -n 's/\([0-9]*\.[0-9]*\) .*/\1 seconds/p' | cat >>$LOG
 
# should be symbolically linked from top C-TAP dir
# Soon we'll experiment more with this filter, maybe better config way.
yuv_filter_cmd="./YUVToBMPStreamFilter   $width $height < PIPE.yuv > PIPE.bmp 2>>$LOG"
( echo ; echo $yuv_filter_cmd ; echo ) | >>$LOG

# width and height are necessary since yuv frames are raw.
# so we have to take care if ffmpeg outputs differently sized
# frames from the movie original.
eval $yuv_filter_cmd &

echo Started YUVToBMPStreamFilter at /proc/uptime= | cat >> $LOG
echo $( cat /proc/uptime ) | sed -n 's/\([0-9]*\.[0-9]*\) .*/\1 seconds/p' | cat >>$LOG


#begin Phase1a and Phase1b

user_options="${OTHER_OPTIONS} ${MOVIE_SCALE_OPTION} ${PIXPROC_SCALE_OPTION} ${USER_SCALE_OPTION}"
#Pass to all although they are not fully used.

#Phase1a

echo $0 STARTED Phase1aPipeOpt /proc/uptime= | cat >> $LOG
echo $( cat /proc/uptime ) | sed -n 's/\([0-9]*\.[0-9]*\) .*/\1 seconds, will block/p' | cat >>$LOG
#echo STARTED Phase1aPipeOpt

#Dont output to a pipe for now, we want to keep the .int file anyway
#so this shell waits don't run in background with & -------- here -------V
Phase1a_cmd="./Phase1aPipeOpt --pipeline ${user_options} 0 10000000 < PIPE.bmp >  ${result_of_1a} 2>> $LOG"
( echo ; echo ${Phase1a_cmd}; echo ) | cat >> $LOG
eval ${Phase1a_cmd}
RET=$?

echo $0 FINISHED returned $RET, /proc/uptime= | cat >> $LOG
echo $( cat /proc/uptime ) | sed -n 's/\([0-9]*\.[0-9]*\) .*/\1 seconds/p' | cat >>$LOG

echo Find Phase1aPipeOpt results in ${result_of_1a} | cat >> $LOG

if [ $RET != 0 ]
then
    echo $Phase1a_cmd
    echo returned status $RET
    echo $0 exiting, bye bye.
    exit $RET
fi

#1000000 is a large number so all the number of frame from the
#movie are processed.  0 causes the frame numbers at the
#start of report lines to begin with 1, for the comparison
#between the first frame, frame 0 and the second, frame 1.

#Phase1b

#We now run Phase1b, again on the .int file as a blocking, file I/O process.

#echo calling Phase1bPipe
#
#./Phase1bPipeOpt PIPE.in 10000000 0.5 ${MOVIE_FILE%.*}.int > ${MOVIE_FILE%.*}.out 

# rm -f ${RESULTS_DIR}/${RESULT_OF_1a}

#for now, the only difference is that Phase1bPipe reads up to $ndiffs lines, so, here:

ndiffs=1000000

#echo "./Phase1bPipeOpt ${RESULTS_DIR}/${RESULT_OF_1a} $ndiffs 0.5 > ${RESULTS_DIR}/${RESULT_OF_1b}"


#echo
#echo DEBUGGING $0
#echo $0 calling:
#echo ./Phase1bPipe ${RESULTS_DIR}/${RESULT_OF_1a} $ndiffs 0.5 '>' ${RESULTS_DIR}/${RESULT_OF_1b_FIRST_TRY} '2>>' $LOG

#no --pipeline options since we are reading from a result .int file

Phase1b_cmd_first_try="./Phase1bPipeOpt  $user_options ${RESULTS_DIR}/${result_of_1a} $ndiffs 0.5 > ${RESULTS_DIR}/${result_of_1b_first_try} 2>> $LOG"
(echo ; echo $Phase1b_cmd_first_try ; echo ) | cat >>$LOG

#run Phase1b
eval $Phase1b_cmd_first_try
ret_1b_first=$?
if [ $ret_1b_first != 0 ]
then
    echo $0 exiting since Phase1bPipeOpt returned error $ret_1b_first.
    echo $0 exiting since Phase1bPipeOpt returned error $ret_1b_first. | cat >> $LOG
    exit 1 #maybe at first
fi

nFIRST=`cat ${RESULTS_DIR}/${result_of_1b_first_try} | wc -l`
echo Find the $nFIRST Phase1b results in ${result_of_1b_first_try} | cat >>$LOG
#echo LOG including Phase1b
#echo "---------------------"
#cat $LOG
#echo "-------end of log----"
#echo
#echo $0 Debugging...
#echo $0 End of debugging for now. We will exit.
#echo $0 End of debugging for now. We will exit | cat >>$LOG
#exit 1
    
    
if [ $nFIRST -gt 50000 ] || [ $nFIRST -eq 0 ]
then
    if [ $nFIRST = 0 ]
    then
	echo "Part deux: electric boogaloo... First try found 0 frames with objects." | cat >>$LOG
	echo "We will do Phase1b a second time with level=0.98 instead of 0.5  In $0 we not redo Phase1a" | cat >>$LOG
    else
	echo "Part deux: electric zittzzeezzz.. First try found $n frames with object, too many" | cat >>$LOG
    fi	


    echo $0 "calling (with level=0.98 instead of 0.5):" | cat >>$LOG
    Phase1b_cmd_second_try="./Phase1bPipeOpt  $user_options ${RESULTS_DIR}/${result_of_1a} $ndiffs 0.98 > ${RESULTS_DIR}/${result_of_1b_second_try} 2>> $LOG"
    ( echo ; echo $Phase1b_cmd_second_try; echo ) | >>$LOG
    #run Phase1b
    eval $Phase1b_cmd_second_try 	
    ret_1b_second=$?

    if [ $ret_1b_second != 0 ]
    then
	echo "Second Phase1bPipe failure return code $ret_1b_second." | cat >>$LOG
	echo exiting | cat >>$LOG
	echo $0 exit because Second Phase1Pipe returned error code $ret_1b_second
	exit 1
    fi
    nSECOND=`cat ${RESULTS_DIR}/${result_of_1b_second_try} | wc -l`
    echo "Phase 1B: Re-Done reports $nSECOND frames with objects." | cat >> $LOG
    echo see ${RESULTS_DIR}/${result_of_1b_second_try} | cat >> $LOG
fi

#rare msgs to user
#echo -------------------------------------------
#echo $0 finishes by dumping the log for you
#cat $LOG
#echo -----------------------------------------
#echo GOODBYE from $0

exit 0
