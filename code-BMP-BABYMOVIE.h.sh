echo SOURCED:  code-BMP-BABYMOVIE.h.sh

pushd ${BITMAPS_PARENT_DIR}

##scaling? x and y are pixel coords.
cat ${RESULTS_DIR}/${RESULT_OF_1b_BASE} | while read evt frame extr x y prob
do
    ((++frame))
    #if (( $frame %5 == 0 ))
    #then
    #echo "processing frame number" $frame
    #fi

#    i=$((x-9))  ##scaling?  maybe 9 could remain unscaled
#    j=$((y-9))
#    k=$((x+9))
#    l=$((y+9))

    i=$((x-18))  ##scaling?  maybe 9 could remain unscaled
    j=$((y-18))
    k=$((x+18))
    l=$((y+18))

    frameDigbmp="$(padTo6Digs $frame).bmp"
    inbmpPaName="${BITMAPS_DIR}/thumb${frameDigbmp}"
    outbmpPaName="${BITMAPS_DIR}/pic${frameDigbmp}"
   
    stkw="2"

    if [ $y -lt 800 ]
    then
	color="cyan"
    else
	color="lime"
    fi

    BITMAP_EDIT_CMD="convert ${inbmpPaName} "
    BITMAP_EDIT_CMD="${BITMAP_EDIT_CMD} -fill none -stroke ${color} -strokewidth ${stkw}"
    BITMAP_EDIT_CMD="${BITMAP_EDIT_CMD} -draw 'circle $i,$j $k,$l' "
    BITMAP_EDIT_CMD="${BITMAP_EDIT_CMD} ${outbmpPaName}"

    eval $BITMAP_EDIT_CMD
done
    
echo "Breaking Lue's NDA...make a movie of them"

cd ${BITMAPS_DIR}
#We just had added the pic*.bmp frames for the 'baby movie'
EXT="MOV" #NOT same as ext above!
#mogrify -resize 640x360 "*" *.bmp #mogrify -format jpg *.bmp
echo "ffmpeg now makes a baby movie." | cat >> ${RESULTS_DIR}/ffmpeg.outputs

make_baby_cmd="ffmpeg -hide_banner -y -threads 0 -r 60 -f image2 -pattern_type glob -i 'pic*.bmp' -vcodec libx264 -crf 25 -pix_fmt yuv420p ${RESULTS_DIR}/${moviePrefix}.${EXT} &>> $RESULTS_DIR/ffmpeg.outputs"
(echo ; echo ${make_baby_cmd}; echo ) | cat >> ${COMMAND_ARCHIVE_PATHNAME}

eval ${make_baby_cmd}



#ffmpeg docs:
#
# -f image2 -pattern_type glob -i 'pic*.bmp'
# is for "demultiplexing" a collection identical format images selected by the filename glob.
#
# -vcodec libx264 -crf 25
#
# -pix_fmt yuv420p  
#
    
#ffmpeg -i ${moviePrefix}.${ext} -filter:v "transpose=1,transpose=1" flipped.mp4
    

popd

