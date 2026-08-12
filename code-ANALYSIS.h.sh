echo SOURCED: code-ANALYSIS.h.sh

#Phase1ab common options
opt_scaling="${MOVIE_SCALE_OPTION} ${PIXPROC_SCALE_OPTION} ${USER_SCALE_OPTION}"

phase1a_options="${opt_scaling} ${OPT_CamSett} ${OTHER_OPTIONS} ${PHASE1A_OTHER_OPTIONS}"
#check if $BITMAPS_DIR is null if we are not reusing bitmaps..I think that's done??
if [ $ARCHITECTURE = "framefile" ]
then
    phase1a_options="${phase1a_options} --bitmaps-dir $BITMAPS_DIR"
    #Note Phase1a --pipeline REJECTS --bitmaps-dir option. 
fi

phase1b_options="${opt_scaling} ${OPT_CamSett} ${OTHER_OPTIONS}"

#for pipelined frames: ffmpeg_pipe_extract input-movie-filename pipe-filename-with-yuv
# It should be a named Unix pipe.
#Writes to ${JOBNAME}PIPE.yuv, the .yuv extension specifies raw format to ffmpeg
#This should run in a shell background (i.e., separate process).
#If not, concurrency will not occur and, what's worse, kernal VM
#might become exhaused from all those raw video
#frames.  (They are 54/2 bytes smaller than half the size of .bmp frames.)
function ffmpeg_pipe_extract() {
    (echo ; echo -n; pwd; echo ffmpeg -hide_banner -y -an -i $1 ${FFMPEG_EXTRACT_FILTER} ${PIPE_DIR}/${JOBNAME}PIPE.yuv '&>'${RESULTS_DIR}/ffmpeg.log ) >> ${COMMAND_ARCHIVE_PATHNAME}
    $time_cmd_prefix ffmpeg -hide_banner -y -an -i $1 ${FFMPEG_EXTRACT_FILTER} ${PIPE_DIR}/${JOBNAME}PIPE.yuv &>>${RESULTS_DIR}/ffmpeg.log
}

#for .bmp frames: ffmpeg_bmp_extract <movie_file-file-name> <logfile>
#Creates one thumb123456.bmp file for each frame, which comprise
#a big mostly boring herd of hogs to occupy your disk; but keeping
#them is convenient to make a movie of the interesting ones.
ffmpeg_bmp_extract() {
    $time_cmd_prefix ffmpeg -hide_banner -y -an -i $1 ${FFMPEG_EXTRACT_FILTER} thumb%06d.bmp &>$2
}
