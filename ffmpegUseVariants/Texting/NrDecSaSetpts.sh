#!/bin/bash

NAME=NrDecSaSetpts

FF=ff

cp /dev/null  $NAME.fflog

if /usr/bin/time -o $NAME.time -v ${FF}mpeg  -i DS.mp4 -an -filter:v 'drawtext=fontfile=arial.ttf:text=%{n}:x=(w-tw)-140:y=(2*lh)-80:fontcolor=white:box=1:boxcolor=0x00000099:fontsize=60,decimate,scale=trunc(iw/4)*2:trunc(ih/4)*2,setpts=N/100/TB' -vsync vfr -y $NAME.mp4 &>> $NAME.fflog
then
    echo Numbering-decimate ffmpeg done.
else
    echo Numbering-decimate ffmpeg failed.
    cat $NAME.fflog
    exit
fi


if /usr/bin/time -o $NAME.time -v ${FF}mpeg  -i $NAME.mp4 -an -filter:v 'drawtext=fontfile=arial.ttf:text=%{n}:x=(w-tw)-200:y=(2*lh)-40:fontcolor=white:box=1:boxcolor=0x00000099:fontsize=30' -y $NAME.numb.mp4 &>> $NAME.fflog 
then
    echo Numbering after num-decimate done.
else
    echo Numbering after Numbering-Decimate ffmpeg failed.
    ${FF}play $NAME.mp4 &
    less $NAME.fflog
    exit
fi


${FF}play $NAME.mp4 &
${FF}play $NAME.numb.mp4 &








