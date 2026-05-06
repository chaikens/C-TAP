#!/usr/bin/bash
#/*
# DoOneMovieHalfDecimated1ab.sh MOVIEFILENAME.mp4 Logfile
#
# Half resolution and decimation coded by ffmpeg filter below.
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


ffmpeg_pipe_extract()
{
    ffmpeg  -hide_banner -y -an   \
	    -i $1                 \
            -vf                   \
            "scale=trunc(iw/4)*2:trunc(ih/4)*2,decimate,setpts=N/100/TB" \
	    PIPE.yuv &> ffmpeg.log
}

     MOVIE_TO_FRAME_DIV=2
     MOVIE_SCALE_OPTION="--movie-scale 1"
     PIXPROC_SCALE_OPTION="--pixproc-scale 1"
     USER_SCALE_OPTION="--user-scale 1"
     OTHER_OPTIONS="--no-crop --camera-index 1"
     #In Phase1a C++ program, we wrote an exclusion zone function for DroneShort1,
     #and also named a camera for it.  
     
source DoOneMoviePipeCommon.h.sh
