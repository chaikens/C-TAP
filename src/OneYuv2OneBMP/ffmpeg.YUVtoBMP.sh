#!/bin/bash
# first arg must be nnnnxmmmm size
if [ "${2##*.}" != "yuv" ]; then
    echo "First arg must be a yuv filename"
    exit
fi

if [ "${3##*.}" != "bmp" ]; then
       echo "3rd arg must be a .yuv filename"
       exit
fi

ffmpeg -hide_banner -y -an -video_size $1 -i $2 -frames:v 1 $3



	 

