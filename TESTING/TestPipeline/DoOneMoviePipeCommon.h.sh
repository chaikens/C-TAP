
#/*
# DoOneMoviePipeCommon.h.sh 
# MUST NOT BE EXECUTABLE
# WILL BE SOURCED BY SCRIPTS
# THAT DEFINE
#
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
#
#
# function modify_linear_resolution()
# {
#   echo $1
# }
#or, respectively, 
# {
#   echo $(($1 / 2))
# }
#
#
if [ $(type -t ffmpeg_pipe_extract) != "function" ] 
then
    echo $0: ffmpeg_pipe_extract must be defined as a 
    echo shell function before sourcing DoOneMoviePipeCommon.h.sh
    exit 1
fi
if [ $(type -t modify_linear_resolution) != "function" ] 
then
    echo $0: modify_linear_resolution  must be defined as a 
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
# ./Phase1aPipe
# ./Phase1bPipe
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
MOVIE_BASE=${MOVIE_FILE%.*} #filename (absolute or relative) without the DOT
RESULT_OF_1a=$MOVIE_BASE.int #NOT the name of the pipe, that's PIPE.int
RESULT_OF_1b_FIRST_TRY=$MOVIE_BASE.out #No piping from that yet, will be PIPE.out
RESULT_OF_1b_SECOND_TRY=$MOVIE_BASE.redo.out

# shell function defined by the script that included me
width=$(modify_linear_resolution  $(widthOfMovie  ${MOVIE_FILE}))
height=$(modify_linear_resolution $(heightOfMovie ${MOVIE_FILE}))

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
echo $( cat /proc/uptime ) | cat >>$LOG
 
# should be symbolically linked from top C-TAP dir
# Soon we'll experiment more with this filter, maybe better config way.
./YUVToBMPStreamFilter   $width $height < PIPE.yuv > PIPE.bmp 2>>$LOG &
# width and height are necessary since yuv frames are raw.
# so we have to take care if ffmpeg outputs differently sized
# frames from the movie original.
echo STARTED YUVToBMPStreamFilter at /proc/uptime= | cat >> $LOG
echo $(cat /proc/uptime) | cat >>$LOG


echo $0 STARTED Phase1aPipe /proc/uptime= | cat >> $LOG
echo $(cat /proc/uptime) "," will block | cat   >> $LOG
#echo STARTED Phase1aPipe

#Dont output to a pipe for now, we want to keep the .int file anyway
#so this shell waits don't run in background with & -------- here -------V
./Phase1aPipe 0 10000000 < PIPE.bmp >  ${RESULT_OF_1a}
RET=$?
echo $0 FINISHED returned $RET, /proc/uptime= | cat >> $LOG
echo $(cat /proc/uptime) | cat >>$LOG
echo Find Phase1a results in ${RESULT_OF_1a} | cat >> $LOG

#1000000 is a large number so all the number of frame from the
#movie are processed.  0 causes the frame numbers at the
#start of report lines to begin with 1, for the comparison
#between the first frame, frame 0 and the second, frame 1.

#We now run Phase1b, again on the .int file as a blocking, file I/O process.

#echo calling Phase1bPipe
#
#./Phase1bPipe PIPE.in 10000000 0.5 ${MOVIE_FILE%.*}.int > ${MOVIE_FILE%.*}.out 

# rm -f ${RESULTS_DIR}/${RESULT_OF_1a}

#for now, the only difference is that Phase1bPipe reads up to $ndiffs lines, so, here:

ndiffs=1000000

#echo "./Phase1bPipe ${RESULTS_DIR}/${RESULT_OF_1a} $ndiffs 0.5 > ${RESULTS_DIR}/${RESULT_OF_1b}"

echo "./Phase1bPipe ${RESULTS_DIR}/${RESULT_OF_1a} $ndiffs 0.5 > ${RESULTS_DIR}/${RESULT_OF_1b}" | cat >>$LOG



#echo
#echo DEBUGGING $0
#echo $0 calling:
#echo ./Phase1bPipe ${RESULTS_DIR}/${RESULT_OF_1a} $ndiffs 0.5 '>' ${RESULTS_DIR}/${RESULT_OF_1b_FIRST_TRY} '2>>' $LOG

./Phase1bPipe ${RESULTS_DIR}/${RESULT_OF_1a} $ndiffs 0.5 > ${RESULTS_DIR}/${RESULT_OF_1b_FIRST_TRY} 2>> $LOG
RET_1b_FIRST=$?
if [ $RET_1b_FIRST != 0 ]
then
    echo $0 exiting since Phase1bPipe returned error $RET_1b_FIRST.
    echo $0 exiting since Phase1bPipe returned error $RET_1b_FIRST. | cat >> $LOG
    exit 1 #maybe at first
fi

nFIRST=`cat ${RESULTS_DIR}/${RESULT_OF_1b_FIRST_TRY} | wc -l`
echo Find the $nFIRST Phase1b results in ${RESULT_OF_1b_FIRST_TRY} | cat >>$LOG
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
	echo $0 "calling (with level=0.98 instead of 0.5):" | cat >>$LOG
	echo "./Phase1bPipe ${RESULTS_DIR}/${RESULT_OF_1a} $ndiffs 0.98 '>' ${RESULTS_DIR}/${RESULT_OF_1b_SECOND_TRY} '2>>' $LOG" | cat >> $LOG
    fi	
    
    ./Phase1bPipe ${RESULTS_DIR}/${RESULT_OF_1a} $ndiffs 0.98 > ${RESULTS_DIR}/${RESULT_OF_1b_SECOND_TRY} 2>> $LOG
	
    RET_1b_SECOND=$?

    if [ $RET_1b_SECOND != 0 ]
    then
	echo "Second Phase1bPipe failure return code $RET_1b_SECOND." | cat >>$LOG
	echo exiting | cat >>$LOG
	echo $0 exit because Second Phase1Pipe returned error code $RET_1b_SECOND
	exit 1
    fi
    nSECOND=`cat ${RESULTS_DIR}/${RESULT_OF_1b_SECOND_TRY} | wc -l`
    echo "Phase 1B: Re-Done reports $nSECOND frames with objects." | cat >> $LOG
    echo see ${RESULTS_DIR}/${RESULT_OF_1b_SECOND_TRY} | cat >> $LOG
fi

#rare msgs to user
#echo -------------------------------------------
#echo $0 finishes by dumping the log for you
#cat $LOG
#echo -----------------------------------------
#echo GOODBYE from $0

exit 0
