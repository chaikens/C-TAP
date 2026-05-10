#!/bin/bash
if [ "${1##*.}" != "bmp" ]; then
    echo "First arg must be a .bmp"
else
    if [ "${2##*.}" != "yuv"; then
	   echo "2nd arg must be a .yuv filename"
	 else
	     ffmpeg -hide_banner -y -i $1 -pix_fmt yuv420p $2
       fi
fi

	 

