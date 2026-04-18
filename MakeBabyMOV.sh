#!/usr/bin/bash
#
##
# This script is called MakeBabyMOV.sh because the extension
# of the results filename is .MOV (capitalized, originally to
# distinguish it from input movies.

moviePrefix=$1
shift 1

#ugly mid-20th century antique obsoleteness 
if [ " "$1 = " ""--bmpsdir" ]
then
    shift 1
    BITMAPS_DIR=$1
    echo "in arg analysis, BITMAPS_DIR=" ${BITMAPS_DIR}
    #maybe check for readable dir.
    shift 1
else
    #for now, we only do v1.0 stuff
    echo arg1 is not --bmpsdir
    exit 1
fi

PHASE1B_OUT_FILE=$1
if [ ""${PHASE1B_OUT_FILE} = "" ]
then
    echo missing Phase1b .out file
    exit 1
fi
#maybe check for readable file with .out ext?
shift 1

RESULTS_DIR=$1
if [ ""${RESULTS_DIR} = "" ]
then
    echo missing results dir.
    exit 1
fi

echo
echo MakeBabyMov.sh called with
echo $moviePrefix
echo --bmpsdir ${BITMAPS_DIR}
echo $PHASE1B_OUT_FILE
echo $RESULTS_DIR
echo begin baby movie making
echo


    echo "Logging range, deploying Little Green Men.."
    cat ${PHASE1B_OUT_FILE} | while read evt frame extr x y prob
    do
	((++frame))
	if (( $frame %5 == 0 ))
	then
	    #hmff.. obscure ANSI-C Quoting feature of bash
	    # no trailing newline and a carriage return to overlap lines
	    echo -n $'\r' "processing frame number $frame"
	fi
	i=$((x-9))
	j=$((y-9))
	k=$((x+9))
	l=$((y+9))
	if [ $frame -lt 10 ]
        then
            pad="00000"
        elif [ $frame -lt 100 ]
        then
            pad="0000"
        elif [ $frame -lt 1000 ]
	then
            pad="000"
        elif [ $frame -lt 10000 ]
        then
            pad="00"
        elif [ $frame -lt 100000 ]
        then
            pad="0"
        else
            pad=""
        fi

	if [ $y -lt 800 ]
	then
	    # echo '$y -lt 800 yes branch'
	    BITMAP_EDIT_CMD=\
"convert ${BITMAPS_DIR}/thumb$pad$frame.bmp -fill none -stroke cyan -draw 'circle $i,$j $k,$l' ${BITMAPS_DIR}/pic$pad$frame.bmp"
	else
	    # echo  '$y -lt 800 no branch'
	    BITMAP_EDIT_CMD=\
"convert ${BITMAPS_DIR}/thumb$pad$frame.bmp -fill none -stroke lime -draw 'circle $i,$j $k,$l' ${BITMAPS_DIR}/pic$pad$frame.bmp"
	fi
	# ..echo 'We will eval' $BITMAP_EDIT_CMD
	eval $BITMAP_EDIT_CMD
    done

    echo #print newline after overlapped reports
    echo "Breaking Lue's NDA..."
    cd ${BITMAPS_DIR}
    EXT="MOV" #NOT same as ext above!
    #mogrify -resize 640x360 "*" *.bmp #mogrify -format jpg *.bmp
    #  'pic*.bmp' must use single quotes so bash itself doesn't do filename extpansion 
    ffmpeg -hide_banner -y -threads 0 -r 60 -f image2 -pattern_type glob -i 'pic*.bmp' -vcodec libx264 -crf 25 -pix_fmt yuv420p ${RESULTS_DIR}/${moviePrefix}.${EXT}

    #ffmpeg docs:
    #
    # -f image2 -pattern_type glob -i 'pic*.bmp'
    # is for "demultiplexing" a collection identical format images selected by the filename glob.
    #
    # -vcodec libx264 -crf 25
    #
    # -pix_fmt yuv420p 
    #

    ffmpeg_ret=$?
    echo ffmpeg return status = $?
    
    #ffmpeg -i ${moviePrefix}.${ext} -filter:v "transpose=1,transpose=1" flipped.mp4

    #will someday have return 1 on any failure    

    exit $ffmpeg_ret
   



