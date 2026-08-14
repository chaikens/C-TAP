#!/bin/bash

FF=ff

FFOPT="-hide_banner -loglevel error -threads 4"

srcdir=$1
movie=$2
dstdir=$3

mkdir -p $dstdir


#${FF}mpeg  -i $srcdir/$movie -an -filter:v 'drawtext=fontfile=arial.ttf:text=%{n}:x=(w-tw)-140:y=(2*lh)-80:fontcolor=white:box=1:boxcolor=0x00000099:fontsize=60,scale=trunc(iw/4)*2:trunc(ih/4)*2,decimate=cycle=5,setpts=N/100/TB' -vsync vfr -y $dstdir/temp.mp4 &>>$movie.log

${FF}mpeg $FFOPT -i $srcdir/$movie -an -filter:v 'drawtext=fontfile=arial.ttf:text=%{n}:x=(w-tw)-140:y=(2*lh)-80:fontcolor=white:box=1:boxcolor=0x00000099:fontsize=60,scale=trunc(iw/4)*2:trunc(ih/4)*2,decimate=cycle=5,setpts=N/100/TB' -vsync vfr -y $dstdir/$movie &>>${dstdir}/${movie}.log

#${FF}mpeg -i $dstdir/temp.mp4 -an -filter:v 'drawtext=fontfile=arial.ttf:text=%{n}:x=(w-tw)-200:y=(2*lh)-40:fontcolor=white:box=1:boxcolor=0x00000099:fontsize=30' -vsync vfr -y $dstdir/$movie &>>$movie.log



#${FF}mpeg $FFOPT -i $srcdir/$movie -an -filter:v  'drawtext=fontfile=arial.ttf:text=%{n}:x=(w-tw)-140:y=(2*lh)-80:fontcolor=white:box=1:boxcolor=0x00000099:fontsize=60,scale=trunc(iw/4)*2:trunc(ih/4)*2,decimate,drawtext=fontfile=arial.ttf:text=%{n}:x=(w-tw)-200:y=(2*lh)-40:fontcolor=white:box=1:boxcolor=0x00000099:fontsize=30' -y $dstdir/$movie 

#'drawtext=fontfile=arial.ttf:text=%{n}:x=(w-tw)-140:y=(2*lh)-80:fontcolor=white:box=1:boxcolor=0x00000099:fontsize=60,scale=trunc(iw/4)*2:trunc(ih/4)*2,decimate,setpts=N/100/TB,drawtext=fontfile=arial.ttf:text=%{n}:x=(w-tw)-200:y=(2*lh)-40:fontcolor=white:box=1:boxcolor=0x00000099:fontsize=30' -vsync vfr -y $dstdir/$movie 

#'drawtext=fontfile=arial.ttf:text=%{n}:x=(w-tw)-140:y=(2*lh)-80:fontcolor=white:box=1:boxcolor=0x00000099:fontsize=60,scale=trunc(iw/4)*2:trunc(ih/4)*2,setpts=N/100/TB' -y $dstdir/temp.mp4 




#${FF}mpeg $FFOPT -i $dstdir/temp.mp4 -an -filter:v 'drawtext=fontfile=arial.ttf:text=%{n}:x=(w-tw)-200:y=(2*lh)-40:fontcolor=white:box=1:boxcolor=0x00000099:fontsize=30' -y $dstdir/$movie

#rm $dstdir/temp.mp4











