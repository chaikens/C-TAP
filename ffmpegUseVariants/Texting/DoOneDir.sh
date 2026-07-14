#!/bin/bash

PATH=`pwd`:$PATH

DoOneCmd="OneMovie.sh"
Dope="Two successive ffmpegs(-vsync vfr): 1. OrigFn, Scale, Decimate, Setpts. 2. Dec Fn"

DIR=$1  #should be a single token, please run in dir with this subdir.
DESTDIR=PP-$1

mkdir -p $DESTDIR

for mov in ${DIR}/N*.mp4
#This pattern excludes the .Nblaaablaa.mp4 files made by bad apples. 
do
    echo $mov
    mov=$(basename $mov)
    echo $mov
    echo $dope
    date
    $DoOneCmd $DIR $mov $DESTDIR
done

