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
# ./Phase1aPipe
# ./Phase1bPipe
#*/

function ffmpeg_pipe_extract() {
    ffmpeg -hide_banner -y -i $@ PIPE.yuv &> ffmpeg.log
    }

function modify_linear_resolution() {
    echo $1
    }

source DoOneMoviePipeCommon.h.sh

