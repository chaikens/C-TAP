#!/usr/bin/bash

function widthOfMovie() {
    ffprobe -v quiet  -show_streams $@ | sed -n 's/^width=\([1-9][0-9]*\)$/\1/p'
}

function heightOfMovie() {
    ffprobe -v quiet  -show_streams $@ | sed -n 's/^height=\([1-9][0-9]*\)$/\1/p'
    }


MOVIE_FILE=$1

width=$(widthOfMovie ${MOVIE_FILE})
height=$(heightOfMovie ${MOVIE_FILE})

yuvsizeb=$(( (width*height*3)/2 ))

echo width=${width} height=${height} yuvsizeb=${yuvsizeb}

echo Phase1aPipeBMP.sh Processing ${width}x${height} $MOVIE_FILE



rm -f PIPE.yuv
mknod PIPE.yuv p

rm -f PIPE.bmp
mknod PIPE.bmp p

 ffmpeg -hide_banner -y -i $MOVIE_FILE PIPE.yuv &> ffmpeg.log &

./YUVToBMPStreamFilter   $width $height < PIPE.yuv > PIPE.bmp &

./Phase1aPipe  0 99999 < PIPE.bmp > ${MOVIE_FILE%.*}.int













