#!/bin/bash

PATH=`pwd`:$PATH

DoOneCmd="OneMovieScDecSetptsOnly.sh"
Dope="One ffmpeg: 1. OrigFn, Scale, Decimate, Setpts."

DIR=$1  #should be a single token, please run in dir with this subdir.
DESTDIR=PP-$1

mkdir -p $DESTDIR

for mov in ${DIR}/N*.mp4  #This pattern excludes the .Nblaaablaa.mp4 files made by bad apples. 
do
    echo $mov
    mov=$(basename $mov)
    echo $mov
    echo $Dope
    date
    $DoOneCmd $DIR $mov $DESTDIR
done

