#!/bin/bash -x
#
# <this script>  movie.mp4 dd
# writes to FRAME0..dd.yuv
#
echo $0
#if ! make yuvSelectMulti
#then
#    exit 1
#fi

source HELPERS.h.sh

movf=$1
franum=$2
outfile=FRAME$(padTo6Digs $franum).yuv

w=$(widthOfMovie $movf)
h=$(heightOfMovie $movf)

wantf="movtoayuvframe.nums"

(echo ; echo $franum; echo ) | cat > $wantf

rm -f PIPE.yuv
mknod PIPE.yuv p

./yuvSelectMulti  --verbose "${w}x${h}" 3 4 --yuv-out-fd 5 3< ${wantf} 4< PIPE.yuv  5> $outfile &

ffmpeg -hide_banner -y -an -i $movf PIPE.yuv
