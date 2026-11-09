#!/bin/bash

# PPOneDir.sh   D DEST
#    make dir DEST/PP-D;
#    use PPOneMovie.sh put preprocessed movies from D
#    into same named movies in DEST/PP-D
#    Specify preprocessing command(s) in the PPOneMovie.sh alongside this script.

# PPOneDir.sh   D (dir of input movies)
#    Use D for DEST above.

# PPOneDir.sh
#    Use cwd for D and DEST above.

#optional name of dir for PP-$DIR output directory
OPTDESTDIRDIR=dir_of_PP_dirs

SRCDIR=${1-$(pwd)}  #should be a single token, please run in dir with this subdir.
SRCBASE=$(basename $SRCDIR)

DESTDIRDIR=${2-${SRCDIR}}

DESTDIR=${DESTDIRDIR}/PP-$SRCBASE

mkdir -p $DESTDIR

for mov in ${SRCDIR}/*.mp4  #This pattern excludes the .Nblaaablaa.mp4 files made by bad apples. 
do
    mov=$(basename $mov)
    #echo $mov
    $(dirname $0)/PPOneMovie.sh $SRCDIR $mov $DESTDIR
done

