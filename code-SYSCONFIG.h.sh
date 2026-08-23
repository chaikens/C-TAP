echo SOURCED: code-SYSCONFIG.h.sh


if [ $ARCHITECTURE = "pipeline" ]
then
    #echo DEBUG: code-SYSCONFIG pipeline branch
    unset REUSE_BMPS
    if [ ${NEW_BABY_DEVEL} = "no" ]
    then
	#echo DEBUG: making bitmaps dir in NEW_BABY_DEVEL = no branch.
	if ! mkdir -p $BITMAPS_DIR
	then
	    echo Cant make BITMAPS_DIR $BITMAPS_DIR
	    exit 1
	fi
    else
	unset BITMAPS_DIR
    fi
fi

if [ $ARCHITECTURE = "framefile" ]
then
    unset PIPE_DIR  #Maybe we'll use pipes for something else in the future.
    #echo DEBUG: making bitmaps dir in framefile branch.
    if ! mkdir -p $BITMAPS_DIR
    then
	echo Cant make BITMAPS_DIR $BITMAPS_DIR
	exit 1
    fi
fi

if [ ${KILLALL_XTERMS_AT_START}"xx" = "yesxx" ]
then
    killall -q xterm
fi
