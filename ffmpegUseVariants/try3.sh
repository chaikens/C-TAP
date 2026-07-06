
LOG=log.$(basename $0)

time_cmd_prefix=

FFMPEG_EXTRACT_FILTER="-vf scale=trunc(iw/4)*2:trunc(ih/4)*2,decimate,setpts=N/100/TB" 

ffmpeg_bmp_extract() {
    $time_cmd_prefix ffmpeg -hide_banner -y -an -i $1 ${FFMPEG_EXTRACT_FILTER} -r 1 thumb%06d.jpeg &>$2
}

ffmpeg_bmp_extract DroneShort1.mp4 $LOG &

tail -f $LOG




