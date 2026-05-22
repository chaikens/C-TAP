echo SOURCED: code-SYSCONFIG.h.sh

if [ $ARCHITECTURE = "pipeline" ]
then
    unset REUSE_BMPS BITMAPS_DIR_NAME BITMAPS_PARENT_DIR BITMAPS_DIR
    MAKE_BABY_MOVIE="no"
elif [ $ARCHITECTURE = "framefile" ]
then
    unset PIPE_DIR
fi
