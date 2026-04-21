#!/usr/bin/bash


ffmpeg_pipe_extract()
{
    ffmpeg  -hide_banner -y -an   \
	    -i $1                 \
            -vf                   \
            "scale=trunc(iw/4)*2:trunc(ih/4)*2,decimate,setpts=N/100/TB" \
	    PIPE.yuv &> ffmpeg.log
}


function modify_linear_resolution()
{
    echo $(($1 / 2))
}

source DoOneMoviePipeCommon.h.sh


