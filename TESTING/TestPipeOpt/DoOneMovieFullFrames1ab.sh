#!/usr/bin/bash
#/*
# DoOneMovieFullFrames1ab.sh MOVIEFILENAME.mp4 Logfile
#
# FullFrames since ffmpeg below only does default convers to I420p yuv raw.
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

function ffmpeg_pipe_extract() {
    ffmpeg -hide_banner -y -i $@ PIPE.yuv &> ffmpeg.log
    }

     MOVIE_TO_FRAME_DIV=1
     MOVIE_SCALE_OPTION="--movie-scale 2"
     PIXPROC_SCALE_OPTION="--pixproc-scale 1"
     USER_SCALE_OPTION="--user-scale 1"
     OTHER_OPTIONS="--no-crop --camera-index 1"
     #In Phase1a C++ program, we wrote an exclusion zone function for DroneShort1,
     #and also named a camera for it.  

source DoOneMoviePipeCommon.h.sh
