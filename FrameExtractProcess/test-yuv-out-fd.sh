#!/bin/bash
echo $0
if ! make yuvSelectMulti
then
    exit 1
fi
source HELPERS.h.sh

movf="DroneShort1.mp4"          #$1 #for future use
fwantfile="DroneShort1Full.out" #$2 #for future use

w=$(widthOfMovie $movf)
h=$(heightOfMovie $movf)

rm -f PIPE.yuv
mknod PIPE.yuv p

echo
echo Starting yuvSelectMulti in background.
echo raw movie is stored in file select${w}x${h}.yuv since you must specify dims. to play it.
startalltime=$(uptimenow)
echo $startalltime
echo

(/usr/bin/time --verbose ./yuvSelectMulti --seline-fmt '%*d %d %*d %*d %*d %*f' --yuv-out-fd 5  \
		  "${w}x${h}" 3 4 \
		  3<${fwantfile} 4<PIPE.yuv 5>select${w}x${h}.yuv; \
                  echo; echo yuvSelectMulti stopped; uptimenow; echo ) &

echo Starting ffmpeg in foreground now
uptimenow
echo

/usr/bin/time --verbose \
ffmpeg -hide_banner -loglevel quiet -y -an -i $movf PIPE.yuv

echo 
echo ffmpeg exits

endalltime=$(uptimenow)
echo $endalltime
echo 'Total time (sec)'
echo $(numdif $endalltime $startalltime)


