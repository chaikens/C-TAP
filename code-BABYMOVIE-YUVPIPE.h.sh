
pipe_yuv_to_filter="${PIPE_DIR}/${JOBNAME}PIPE.yuv" #should end in .yuv to tell ffmpeg format
#This pipe filename expression is hardcoded in our shell fn ffmpeg_pipe_extract,
#which also was used for Phase1a extraction:  Same ffmpeg filter and other parameters.

rm -f ${pipe_yuv_to_filter} 
mknod ${pipe_yuv_to_filter} p

pipe_yuv_from_filter="${PIPE_DIR}/${JOBNAME}PIPE2.yuv" #should end in .yuv to tell ffmpeg format
rm -f ${pipe_yuv_from_filter}  
mknod ${pipe_yuv_from_filter} p

echo "Starting baby making yuv extract-->BabyFilter--->MOV pipeline"


if [ -${xterm_ffmpeg_pid}x = "x" ]
then
    touch ${RESULTS_DIR}/ffmpeg.log
    xterm -bg '#E0FFFF' ${XTERM_PARAM} -geometry 160x30+0+100 -title 'ffmpeg extract bitmaps'  -e tail -f ${RESULTS_DIR}/ffmpeg.log &
    #xterm -geometry 160x30+0+100 -title 'ffmpeg extract bitmaps'  -e less -f +F ${RESULTS_DIR}/ffmpeg.log &
    xterm_ffmpeg_pid=$!  #to ensure just one xterm for ffmpeg; may kill at end.
fi

sttime=$(uptimenow)
echo "TIME: Baby pipeline ffmpeg->yuv->BabyFilter->ffmpeg->MOV began " $sttime "sec." >> $LOG

# 3 Stage Pipeline
# Stage 1
$(ffmpeg_pipe_extract ${movie_file}) &
#ffmpeg_pipe_extract shell function already has $time_cmd_prefix and ffmpeg.log logging.

imgdim=${width}x${height}

#Stage 2
#BabyFilter hard codes fd=3 for yuv input, fd=4 for yuv output
${time_cmd_prefix} ${SOFTWARE_DIR}/BabyFilter --phase-1b-file ${phase1b_out} --offset 1 ${imgdim} 3< ${pipe_yuv_to_filter} 4> ${pipe_yuv_from_filter} 2>>${LOG} &

#Stage 3
${time_cmd_prefix} ffmpeg -hide_banner -y -s $imgdim -r 60 -i $pipe_yuv_from_filter  -crf 25  "${RESULTS_DIR}/${generic_baby_movie_name}" &>>${RESULTS_DIR}/ffmpeg.log

#from the original baby making code,
#$FFMPEG -s $S -threads 0 -r 60  -i $SrcToBabyPipe  -vcodec libx264 -crf 25 -pix_fmt yuv420p $BABYMOV
#                                   dont need.......|-------------|
#$FFMPEG -s $S -threads 0 -r 60  -i $SrcToBabyPipe   -crf 25 -pix_fmt yuv420p $BABYMOV
#                                   dont need................|--------------|

rm -f ${pipe_yuv_to_filter} ${pipe_yuv_from_filter}

echo "TIME: Baby pipeline runtime was " $(numdif $(uptimenow) $sttime) " sec." >> ${LOG}
echo "Baby making pipeline finished, " $(numdif $(uptimenow) $sttime) " sec." 

