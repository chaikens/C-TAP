#!/usr/bin/bash
rm -f PIPE.yuv
mknod PIPE.yuv p

coproc YUVER { ffmpeg -hide_banner -y -i DQS.mp4 PIPE.yuv  &
	       dd if=PIPE.yuv bs=2160 ; }

NN=0
while echo $((NN++))  
do
    # ${YUVER[0]} is a numeric Unix file descriptor
    # discovered by echoing that this test for loop ending works.
    if [ "${YUVER[0]}" = "" ]
    then
	break
    fi
    dd iflag=fullblock bs=2160 count=$((3840*3)) of="DQS.fmgPipe.$NN.yuv" <& ${YUVER[0]}
    ffmpeg -hide_banner -y -video_size 3840x2160 -i "DQS.fmgPipe.$NN.yuv" -frames:v 1 "DQS.fmgPipe.$NN.bmp"
done

    
