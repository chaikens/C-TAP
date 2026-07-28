#!/bin/bash


FF=ff
FFOPT="-hide_banner -y "
FFMPEG="${FF}mpeg ${FFOPT}"

SrcToProcPipe="SrcToProcPipe.yuv"
rm -f $SrcToProcPipe
mknod $SrcToProcPipe p

ProcToBabyPipe="ProcToBabyPipe.yuv"
rm -f $ProcToBabyPipe
mknod $ProcToBabyPipe p



#Source movie
SRCMOV=PP-DS.mp4
BABYMOV=PP-DS.MOV

S=1920x1080 #needed because raw I240p yuv carries no metadata incl. format, dims, etc.
            #ffmpeg tells type from filename extension .yuv

$FFMPEG -i $SRCMOV $SrcToProcPipe &

cat < $SrcToProcPipe > $ProcToBabyPipe &

$FFMPEG -s $S -threads 0 -r 60  -i $ProcToBabyPipe   -crf 25  $BABYMOV

#from the original baby making code,
#$FFMPEG -s $S -threads 0 -r 60  -i $SrcToBabyPipe  -vcodec libx264 -crf 25 -pix_fmt yuv420p $BABYMOV
#                                   dont need.......|-------------|
#$FFMPEG -s $S -threads 0 -r 60  -i $SrcToBabyPipe   -crf 25 -pix_fmt yuv420p $BABYMOV
#                                   dont need................|--------------|




