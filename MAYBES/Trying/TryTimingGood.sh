#!/usr/bin/bash
rm -f PIPE.yuv
mknod PIPE.yuv p

coproc YUVER { ffmpeg -hide_banner -y -i DroneShort1.mp4 PIPE.yuv &> YUVER.messages &
	       dd if=PIPE.yuv bs=2160 ; }

NN=0
while echo $((NN++))  
do
    dd iflag=fullblock bs=2160 count=$((3840*3)) of="/dev/null" <& ${YUVER[0]}
#    ffmpeg -hide_banner -y -video_size 3840x2160 -i DS$NN.yuv -frames:v 1 DS$NN.bmp
done




    
