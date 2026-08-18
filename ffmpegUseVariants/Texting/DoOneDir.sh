#!/bin/bash

PATH=`pwd`:$PATH

DIR=$1  #should be a single token, please run in dir with this subdir.
DESTDIR=PP-$1

mkdir -p $DESTDIR

for mov in ${DIR}/N*.mp4  #This pattern excludes the .Nblaaablaa.mp4 files made by bad apples. 
do
    echo $mov
    mov=$(basename $mov)
    echo $mov
    echo OneMovie.sh Two ffmpegs: 1. OrigFn, decimate. 2. Draw Fn on decimated.  
    date
    OneMovie.sh $DIR $mov $DESTDIR
done

