#!/usr/bin/bash
#/*
# Phase1aDoOneMovieFullFrames.sh MOVIEFILENAME.mp4
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
#*/


function widthOfMovie() {
    ffprobe -v quiet  -show_streams $@ | sed -n 's/^width=\([1-9][0-9]*\)$/\1/p'
}

function heightOfMovie() {
    ffprobe -v quiet  -show_streams $@ | sed -n 's/^height=\([1-9][0-9]*\)$/\1/p'
    }


MOVIE_FILE=$1
if [ ! -r ${MOVIE_FILE} ];then echo Cant read movie file; exit; fi

width=$(widthOfMovie ${MOVIE_FILE})
height=$(heightOfMovie ${MOVIE_FILE})

yuvsizeb=$(( (width*height*3)/2 ))

echo width=${width} height=${height} yuvsizeb=${yuvsizeb}

echo $0 Processing ${width}x${height} $MOVIE_FILE

rm -f PIPE.yuv
mknod PIPE.yuv p

rm -f PIPE.bmp
mknod PIPE.bmp p

 ffmpeg -hide_banner -y -i $MOVIE_FILE PIPE.yuv &> ffmpeg.log &
 #Will experiment with more parameters.  The original C-TAP
 # cut back the resolution.

 
./YUVToBMPStreamFilter   $width $height < PIPE.yuv > PIPE.bmp &
# width and height are necessary since yuv frames are raw.
# so we have to take care if ffmpeg outputs differently sized
# frames from the movie original.

./Phase1aPipe  0 1000000 < PIPE.bmp > ${MOVIE_FILE%.*}.int
#1000000 is a large number so all the number of frame from the
#movie are processed.  0 causes the frame numbers at the
#start of report lines to begin with 1, for the comparison
#between the first frame, frame 0 and the second, frame 1.













