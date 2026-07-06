
LOG=log.$(basename $0)

time_cmd_prefix=

FFMPEG_EXTRACT_FILTER="-vf scale=trunc(iw/4)*2:trunc(ih/4)*2,decimate,setpts=N/100/TB" 

ffmpeg_bmp_extract() {
    $time_cmd_prefix ffmpeg -hide_banner -y -an -i $1 ${FFMPEG_EXTRACT_FILTER} -vcodec:0 lib264 -crf 25 -pix_fmt yuv420p -f data   pipe:1   2>$2 | cat foo.foo
}

ffmpeg_bmp_extract DroneShort1.mp4 $LOG &

tail -f $LOG




