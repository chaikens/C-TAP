echo SOURCED: code-SYSCONFIG.h.sh


if [ $ARCHITECTURE = "pipeline" ]
then
    if [ ${NEW_BABY_DEVEL} = "no" ]
    then
	if ! mkdir -p $BITMAPS_DIR
	then
	    echo Cant make BITMAPS_DIR $BITMAPS_DIR
	    exit 1
	fi
    fi
fi

if [ $ARCHITECTURE = "pipeline" ]
then
    unset REUSE_BMPS
    #we too can now MAKE_BABY_MOVIE s
elif [ $ARCHITECTURE = "framefile" ]
then
    unset PIPE_DIR  #Maybe we'll use pipes for something else in the future.
fi

if [ ${KILLALL_XTERMS_AT_START}"xx" = "yesxx" ]
then
    killall -q xterm
fi
