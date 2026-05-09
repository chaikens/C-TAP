echo SOURCED: code-ANALYSIS.h.sh

#check if $BITMAPS_DIR is null
opt_phase1a_bitmaps="--bitmaps-dir $BITMAPS_DIR"
opt_scaling="${MOVIE_SCALE_OPTION} ${PIXPROC_SCALE_OPTION} ${USER_SCALE_OPTION}"

#for pipelined frames: ffmpeg_pipe_extract input-movie-filename
# PIPE.yuv should be a named Unix pipe.
#Writes to PIPE.yuv, the .yuv extension specifies raw format to ffmpeg
#This should run in a shell background (i.e., separate process).
#If not, concurrency will not occur and, what's worse, kernal VM
#might become exhaused from all those raw video
#frames.  (They are 54/2 bytes smaller than half the size of .bmp frames.)
function ffmpeg_pipe_extract() {
    ffmpeg -hide_banner -y -an -i $1 ${FFMPEG_EXTRACT_FILTER} PIPE.yuv &>ffmpeg.log
}

#for .bmp frames: ffmpeg_bmp_extract <movie_file-file-name> <logfile>
#Creates one thumb123456.bmp file for each frame, which comprise
#a big mostly boring herd of hogs to occupy your disk; but keeping
#them is convenient to make a movie of the interesting ones.
ffmpeg_bmp_extract() {
    ffmpeg -hide_banner -y -an -i $1 ${FFMPEG_EXTRACT_FILTER} thumb%06d.bmp &>$2
}
