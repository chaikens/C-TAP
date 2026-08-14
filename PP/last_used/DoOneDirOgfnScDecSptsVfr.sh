#!/bin/bash

PATH=`pwd`:$PATH

DIR=$1  #should be a single token, please run in dir with this subdir.
DESTDIRDIR=/home/ctap/BENCAM-COPY
DESTDIR=${DESTDIRDIR}/PP-$1

mkdir -p $DESTDIR

for mov in ${DIR}/N*.mp4  #This pattern excludes the .Nblaaablaa.mp4 files made by bad apples. 
do
    mov=$(basename $mov)
    echo $mov
    OneMovieOgfnScDecSptsVfr.sh $DIR $mov $DESTDIR
done

