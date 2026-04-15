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


coproc YUVER {  ffmpeg -hide_banner -y -i $MOVIE_FILE PIPE.yuv &> ffmpeg.log &\
	       dd iflag=fullblock bs=${yuvsizeb} if=PIPE.yuv  ; }

#We use a pipe because ffmpeg expects filename for its output destination.
#YUVER's output fd=${YUVER[0]}

#./YUVToBMPStreamFilter $width $height --verbose <& ${YUVER[0]} > BIG.bmp
# this works, at least we get the first frame (haven't looked at the others)
# but what it should be, below, fails:

rm -f PIPE2
mknod PIPE2 p

./Phase1aPipe 0 99999 < PIPE2 > ${MOVIE_FILE%.*}.int &

#./YUVToBMPStreamFilter $width $height <& ${YUVER[0]} | ./Phase1aPipe --verbose 0 99999

./YUVToBMPStreamFilter $width $height <& ${YUVER[0]} > PIPE2 

#./YUVToBMPStreamFilter $width $height --verbose <& ${YUVER[0]} | cat > BIG.bmp
#does't work !












