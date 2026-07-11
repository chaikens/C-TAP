echo SOURCED:  code-BABYMOVIE-BMP.h.sh
#We write -BMP because in the future, we might draw circles, etc into yuv frames!

#The baby movie is composed of frames selected by Phase1b and reported in .out
#The selection is the frame with number 1+index given in .out, which is the
#SECOND frame for Phase1a's difference calculation.

#The code below adds a cirle around the point specified in the .out file
# and then makes a movie of them.

#graphics for the circle

##SHOULD BE SCALED!!
##circles with center (above)< sky_limit are cyan, below lime (ImageMagick coords.)
sky_limit=800
BABY_MOVIE_CIRCLE_RAD_DIV=100;


pushd $BITMAPS_DIR > /dev/null
rm -f pic*.bmp #only delete images used to make the previous "baby movie"
#ffmpeg will input from this glob expression.
#We must delete these even if KEEP_BABY_BMPS==yes because any old pic* frames will be
#included in the baby movie made when ffmpeg inputs pic*.bmp


if [ ${ARCHITECTURE} = "pipeline" ]
then
    #what to use for PIPE.yuv?
    pipe_yuv="${PIPE_DIR}/${JOBNAME}PIPE.yuv" #should end in .yuv to tell ffmpeg format
    rm ${pipe_yuv}  #Need this??
    mknod ${pipe_yuv} p
    echo PIPE_YUV ${pipe_yuv}
    #Use the same ffmpeg_pipe_extract() as before, should be defined.
    echo "Starting MOV making yuv extraction background task."
    echo "INFO: Starting MOV making yuv extraction background task." >> $LOG
    echo "PGM: Same ffmpeg_pipe_extract() runs again." >> $LOG
    if [ -${xterm_ffmpeg_pid}x = "x" ]
    then
	touch ${RESULTS_DIR}/ffmpeg.log
	xterm -bg '#E0FFFF' ${XTERM_PARAM} -geometry 160x30+0+100 -title 'ffmpeg extract bitmaps'  -e tail -f ${RESULTS_DIR}/ffmpeg.log &
	#xterm -geometry 160x30+0+100 -title 'ffmpeg extract bitmaps'  -e less -f +F ${RESULTS_DIR}/ffmpeg.log &
	xterm_ffmpeg_pid=$!  #to ensure just one xterm for ffmpeg; may kill at end.
    fi

    sttime=$(uptimenow)
    echo "TIME: pipe extract select for MOV began " $sttime " sec." >> ${LOG}
    #ffmpeg_pipe_extract shell function already has $time_cmd_prefix
    $(ffmpeg_pipe_extract ${movie_file}) &

    

    echo "yuvSelectMulti will block to make bmps from Phase1b's picked yuv frames. Get coffee and watch ffmpeg xterm."
    ${time_cmd_prefix} ${SOFTWARE_DIR}/yuvSelectMulti ${width}x${height} 3 4 3<${phase1b_out} 4<${pipe_yuv} \
	       --bmp-out-dirpath $BITMAPS_DIR        \
	       --seline-fmt '%*d %d %*d %*d %*d %*f' \
	       --offset 1
    rm ${pipe_yuv}
    echo "yuvSelectMulti done.  Begin making frames with circles in'em."

    echo "TIME: pipe extract select done after " $(numdif $(uptimenow) $sttime) "sec." >> ${LOG}
fi

#When $ARCHITECTURE=framefile, all the needed frames along with all the others are in $BITMAPS_DIR

echo 0 > ${RESULTS_DIR}/foutcount
#The while loop runs in (another) subshell (process, since it's in a pipeline) so vars set there
#are not those of the current shell. It seems the current shell creates the subshell to include
#its params and their values, but the subshell's variable values are not copied back.
#So, we use external (disk!) storage to return the count, and also happen to accumulate it.
#The need was puzzling and Stack Overflow helped inform us.

##scaling? x and y are anti-Microsoft pixel/processing coords, originally y-flipped by Phase1a
firsttime=1
sttime=$(uptimenow)

cat $phase1b_out | while read evt frame extr x y prob
do
    ((++frame))  #We display the SECOND frame of the difference.  Important?
    #^^^ doesn't affect the loop.  Note yuvSelectMulti --offset 1 to comply with here. 

    frameDigbmp="$(padTo6Digs $frame).bmp"
    inbmpPaName="thumb${frameDigbmp}"
    outbmpPaName="pic${frameDigbmp}"

    if [ $firsttime = 1 ]
    then
	#Dont waste time, only look at first .bmp to get radius.
	#Dont rely of an existing var. for the width
	radpix=$(numquotintnz $(widthOfBmp ${inbmpPaName}) ${BABY_MOVIE_CIRCLE_RAD_DIV})
        ct=$(numquotintnz $radpix 6) #circle thickness
	#firsttime=0 #will use firsttime again to time convert
    fi

    #if (( $frame %5 == 0 ))
    #then
    #echo "processing frame number" $frame
    #fi

#    i=$((x-9))  ##scaling?  maybe 9 could remain unscaled
#    j=$((y-9))
#    k=$((x+9))
#    l=$((y+9))

    # i,j Imagemagick draw circle center param.
    i=$x  ##scaling?  maybe 9 could remain unscaled
    j=$y
    # k,l Imagemagick draw circle some point on circumference param.
    k=$((x+radpix))
    l=$y

    if [ $y -lt "$sky_limit" ]
    then
	color="cyan"
    else
	color="lime"
    fi

   if [ -r $inbmpPaName ] #check because offsets might point us to non-existant frames! 
   then

       	if [ ${KEEP_BABY_BMPS}XXX != yesXXX ]
	then
	    #This renames frameNNNNNN.bmp to picNNNNNN.bmp
	    mv  ${inbmpPaName} ${outbmpPaName}
	    #And this directs Imagemagick to input picNNNNN.bmp
	    #so when drawing circles, it replaces the contents.
	    inbmpPaName=${outbmpPaName}
	fi
	
       
	BITMAP_EDIT_CMD="convert ${inbmpPaName} "
	BITMAP_EDIT_CMD="${BITMAP_EDIT_CMD} -alpha remove "
	BITMAP_EDIT_CMD="${BITMAP_EDIT_CMD} -fill none -stroke ${color} -strokewidth ${ct}"
	BITMAP_EDIT_CMD="${BITMAP_EDIT_CMD} -draw 'circle $i,$j $k,$l' "
	BITMAP_EDIT_CMD="${BITMAP_EDIT_CMD} -alpha off ${outbmpPaName}"

	#second use of firsttime, let's time convert but just once
	#so we don't clutter .times.n files with a report for every frame!
	if [ $firsttime = 1 ]
	then
	    #We must use eval or else Imagemagick gets circle, $i, etc as separate params.
	    eval $time_cmd_prefix $BITMAP_EDIT_CMD
	    firsttime=0
	else
	    eval $BITMAP_EDIT_CMD
	fi
	

	temp=$(cat ${RESULTS_DIR}/foutcount); ((temp++)); echo $temp > ${RESULTS_DIR}/foutcount
	echo -n $'\r'"BabyFrame${temp}isOrigFrame${frame}" #cooler progress indicator.


	
   else
    	echo "Warning BABYMOVIE maker tried to use the non-existant frame ${inbmpPaName}"
    	echo "WARN: BABYMOVIE maker tried to use the non-existant frame ${inbmpPaName}" >> $LOG
   fi
done
echo "TIME: To draw circles into .bmps w/Imagemagick took " $(numdif $(uptimenow) $sttime) " sec." >> ${LOG}

bmfcount=$(cat ${RESULTS_DIR}/foutcount)
rm ${RESULTS_DIR}/foutcount

echo  
echo "Breaking Lue's NDA...make a movie of them ${bmfcount} frames. Watch in ffmpeg's xterm."
echo "INFO: ${bmfcount} baby movie frames." >> $LOG
# cwd is ${BITMAPS_DIR}
#We just had added the pic*.bmp frames for the 'baby movie'
EXT="MOV" #NOT same as ext above!
#mogrify -resize 640x360 "*" *.bmp #mogrify -format jpg *.bmp
echo '   -----=====ZZZZZZZZ=====------   '  >> ${RESULTS_DIR}/ffmpeg.log
echo '    ffmpeg now makes a baby movie. '  >> ${RESULTS_DIR}/ffmpeg.log
echo '   -----=====ZZZZZZZZ=====------   '  >> ${RESULTS_DIR}/ffmpeg.log

#ffmpeg uses the .MOV suffix to tell the output format.

#We use a movie name suffixed with .MOV so ffmpeg and ffplay know its type,
#but we eventually keep the file named .MOV.${logn} and symlink .MOV to it.

generic_baby_movie_name="${moviePrefix}.${EXT}"
baby_movie_logn="${generic_baby_movie_name}.${logn}"

#this removes nothing or the previously made symlink.
rm -f $RESULTS_DIR/${generic_baby_movie_name}

make_baby_cmd="ffmpeg -hide_banner -y -threads 0 -r 60 -f image2 -pattern_type glob -i 'pic*.bmp' -vcodec libx264 -crf 25 -pix_fmt yuv420p "${RESULTS_DIR}/${generic_baby_movie_name}" &>> ${RESULTS_DIR}/ffmpeg.log"
(echo ; echo -n "cd "; pwd; echo ${make_baby_cmd}; echo ) | cat >> ${COMMAND_ARCHIVE_PATHNAME}

eval ${time_cmd_prefix} ${make_baby_cmd}

if [ ${KEEP_BABY_BMPS}poo != yespoo ]
then
    echo "INFO: bitmap dir " $(pwd) " df before clearing:" >> $LOG
    df . >> $LOG
    rm *.bmp
    echo "INFO: After:"
    df . >> $LOG
fi



popd > /dev/null #done w/ bitmaps dir


#make a logn file name and a convenience symlink
pushd ${RESULTS_DIR} > /dev/null

#We want to save the baby with a name .$logn
mv $generic_baby_movie_name $baby_movie_logn  #rename what we save

#But, we want ffplay and other mpeg processors to see the .MOV extension,
#so we make the symbolic link.  It will link to the most recently made movie.
ln -s $baby_movie_logn $generic_baby_movie_name
#In addition to this, we generate command ffplay -f mov ..

popd 


echo 'in' $(pwd) code-BABYMOVIE-BMP.h.sh done 
#as before, the shared shell parameter holding the symlinks name is
# ${RESULTS_DIR}/{moviePrefix}.${EXT}
# We use that in exit_greeting() which helps cd to result dir and ffplay the movie.



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
    

