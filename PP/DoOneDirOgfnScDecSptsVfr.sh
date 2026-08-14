#!/bin/bash

# We called this script in an "at" job as follows:
#
# in a directory of daily movie dirs, like 2025-04-28,
# BE SURE, FOR THE SAKE OF THE AT JOB:
#
# PATH=`pwd`:$PATH
#at <when> (like "now + 700 minutes"
# DoOneDirOgfnScDecVfr.sh 2025-04-27 > log.427 &
# DoOneDirOgfnScDecVfr.sh 2025-04-28 > log.428
# ^D (i.e., <EOF>
#
# we had set DESTDIR=/home/ctap/BENCAM-COPY
#but if you run this script as is, it will create
#the PP-.. version of the dir. (like 2025-04-27) you
#call it on.


PATH=`pwd`:$PATH

DIR=$1  #should be a single token, please run in dir with this subdir.
DESTDIRDIR=
DESTDIR=${DESTDIRDIR}/PP-$1

mkdir -p $DESTDIR

for mov in ${DIR}/N*.mp4  #This pattern excludes the .Nblaaablaa.mp4 files made by bad apples. 
do
    mov=$(basename $mov)
    echo $mov
    OneMovieOgfnScDecSptsVfr.sh $DIR $mov $DESTDIR
done

