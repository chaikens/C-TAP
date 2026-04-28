#!/bin/bash
source C-TAP-HELPERS
REUSE_BMPS=yes #Do not change to yes except if you already
# had the thumb*.bmp frames extracted and are re-using
# the script for development and testing of subsequent steps
# for the same movie and extraction choices.

#
# At the top are instructions and manual settings
# 

#IF you're using the original non-pipelined
#version and follow instructions below to use a bigger
#and maybe faster filesystem for one .bmp image
#for each extracted frame, you'll have to customize
#the following for your system.  Otherwise,
#you can ignore my setting. (In Linux, etc
#you can use "df ." in a directory to see its
#filesystem and its capacity.)
FAST_FILESYS_DIR_IF_USED="/media/seth/CTAP"

#settings for debugging or not.
#DEBUG=true
DEBUG=false
#Also, set debugging or not in our Makefile. This script
#runs make.  However it doesn't (yet) pass it an option to compile
#with -g and not -O3 for debugging.

#The non-pipelined system extracts (many many bit) .bmp movie
#frame files, and this takes a lot of time   We support putting them
#in a customized directory that should reside is a big, fast
#filesystem (i.e. disk partition).
#
#If you configure this, you also must specify the location of the CamSett.txt
#file because the default is the cwd of the C++ program runs.
#
#Otherwise, keep these unchanged
#Today, we need to use the big filesystem, so we don't do
#BITMAPS_PARENT_DIR=$(pwd)

#task: check and exit if bad, run and test ret code of df

#NEW_bitmaps=""
#this option is given to Phase1a
#NEW_CamSett=""
#this options is given Phase1a and Phase1b.

#Today, we use an external drive filesystem
BITMAPS_DIR_NAME="BIGbitmaps"
BITMAPS_PARENT_DIR=${FAST_FILESYS_DIR_IF_USED}
BITMAPS_DIR="${BITMAPS_PARENT_DIR}/$BITMAPS_DIR_NAME"
#Again for today's external drive's sake
NEW_bitmaps="--bitmaps-dir ${BITMAPS_PARENT_DIR}/$BITMAPS_DIR_NAME"
NEW_CamSett="--CamSett-file $(pwd)/CamSett.txt"

#
# Please code below where you would like us to
# find 1 or more movies to process, plus
# one name or a pattern for them all.
#
# We can test on our single, short movie by
# putting it (or a symlink) in the same
# dir as the script.
#
SLOW_MOVIE_DIR=$(pwd)

#
# One of our functions is to copy (using rsync)
# movies from a slow filesystem to the one containing the script,
# presumedly a faster one.
# The script detects if the filesystems are the same and
# in that case, skips the copying.  So, when we're out
# of the box (and you put DroneShort1.mp4 alongside the
# script, copying is not done.
#

#file extension our movies have
ext=mp4

#example where we use SLOW_MOVIE_DIR to specify
#the dir (say on a slow external drive) that contains
#a family of movies, say taken by one session of camera shooting.
# 
# Put a shell globbing pattern in the quotes above to select the list
# of full pathnames of one or more movie files.
#
# Some movie file names incorporate the camera name, so we can select
# from one camera, eg ..$path/*_A1.
#
#This script was pre-programmed to test/demonstrate on
#movie DroneShort1.mp4.  See README.md for getting a copy to
#this 125MB movie.  It's short and results in a clear true
#positive case detection..a purposely flown drone in daytime.
#
# We made several symbolic links all to DroneShort1.mp4
# so that we could identify particular scripts for what
# we are comparing, and their results, with the symbolic
# link, i.e., alternative movie name.  The result files
# are named after the movie files basename plus
# extensions .int, .out, .MOV, .mess
#
# You'll find several scripts named C-TAP-Drone*.sh that
# differ only in configuring what to compare.
#
#Here is an example of how to configure a directory
#full of movies for C-TAP's research, not testing.
#movie_files="$SLOW_MOVIE_DIR/N884A6_ch1_main_*.$ext"
#

#
# Our 4 cases
#
#{YUV-extraction->Our pipelined YUV->BMP, ffmpeg->TONS of .bmp images}
#   X (mathematical cartesian product)
#{full frame ffmpeg extraction, half-linear resolution decimated}
#
# This script and is variant are used for 2/4 tests
# 
# (ffmpeg->TONS of .bmp images, full frame ffmpeg extraction)
#  done by C-TAP-DroneShort1Full.sh
#
# (ffmpeg->TONS of .bmp images, half-linear resolution decimated)
#  done by C-TAP-DroneShort1HalfDecimated.sh
#
# for now, the pipelined arch with our own YUV->BMP filter converter
# is done in GIT branch devel-pipeline where the other 2/4 cases
# have been done.  
# The pipeline works much faster and doesn't use excessive disk space.
# We are keeping the .bmp image version up to make comparisons with
# previous analyses.
#OK diff with brother HalfDecimated
# This is the name for full frame ffmpeg extraction.
movie_files="$SLOW_MOVIE_DIR/DroneShort1FullScaling.$ext"
#Alternative:
#OK diff with brother HalfDecimated
#This is the name for half resolution decimated frame ffmpeg extraction.
#movie_files="$SLOW_MOVIE_DIR/DroneShort1HalfDecimated.$ext"

# moviePrefix is going to be the name of the movie file without any extension.
# So, in one example above, it is DroneShort1Full
# Movies from the project have names that embed date, time and camera.
#
# CWD we should be in when calling our programs from this script
SOFTWARE_DIR=$(pwd)

# Where our results are written
RESULTS_DIR="${SOFTWARE_DIR}/RESULTS"
#That makes it convenient to archive them with git, except for movies
#and big images.  The baby movie .MOV is not too big for normal filesystems.

# RESULTS:
#
# Please check out Section 3.2.1 on image analysis for UAP
# research starting on page 11 of https://arxiv.org/pdf/2312.00558
#
# movieprefix.in (result of Phase1a)
#
# movieprefix.out (result of Phase1b)
#
# movieprefix.MOV "baby movie" made by code below
# by drawing a circle into each frame listed by frame number
# given by entry 2 of each line in movieprefix.out.
#
# Notes for V2.0 we hope (with pipelining instead of file stores): 
# We will assume and then check the frame number is exactly
# number of frames up to this one extracted by our input-to-phase1a input
# from ffmpeg, i.e. those ffmpeg with our command didn't skip.
# We hope this assumption will allow us to retrieve those frames
# (instead of using the expensive-to-write-and-keep thumb*.bmps)
# by running the same ffmpeg extraction to a stream of yuvs
# again.
#
#

# (from original comments, edited)
# PRE-REQ: FFMPEG (any v) +ImageMagick (for convert program)
# (Win: command prompt not enough)
# General directions (ReadMe!):

# Make sure you have a minimum of 780GB free on your HD (internal!)
# If impossible: you can still use the code but it will be much slower
# Choose the camera you want to run (A1,A2,A3,A4,B1,B2,B3,B4)
# by setting the movie name or pattern as described above.
#
# Follow that up by altering the tops of FLIRanalysisPhase1aCamX.cpp
# and FLIRanalysisPhase1bCamX.cpp to match the SAME CAMERA NAME (KEY!)
# After editing those source code files, you must of course save them.
# We now have a Makefile and the script calls make to compile them
# if necsssary.
# The script file comes in executable mode, so in Unix, etc
# systems run it with the command:
# ./C-TAP.sh
# (might be different in some systems).
# The script should be run in its own process, not the parent shell,
# so DONT use source (or . (dot) ).

# UFODAP: path, type avi, file, w=1, 360->800, don't make new vid and mv
# instructions for Windows users, if you have errors, go to the bottom:

#if [ ${DEBUG}"" = "yes" ]
#then
#    echo
#    echo We are debugging
#    echo
#    ulimit -c unlimited
#    MAKE_DEBUG=yes #not used yet
#    #deal with this manually for now.
#    REUSE_BMPS=no
#else
#    ulimit -c 0
#    MAKE_DEBUG=no  #not used yet
#fi

cd ${SOFTWARE_DIR}

# our 2 programs the script calls.
# We've retained their original source file names, the exe names
# below come from our Makefile.
Phase1a=Phase1a  #compiled from FLIRanalysisPhase1aCamX.cpp, alongside us.
Phase1b=Phase1b  #compiled from FLIRanalysisPhase1bCamX.cpp, alongside us.
if ! make Phase1a Phase1b
then
    echo make the C++ progs we need failed.  Check this out.
    exit 1
fi


# v1.0 uses ./bitmaps for .bmps
# (1) extracted from movies
# (2) created by this script when making the "baby movie"

if ! mkdir -p $BITMAPS_DIR
then
    echo "Cant make BITMAPS_DIR $BITMAPS_DIR"
    exit 1
fi

if ! mkdir -p $RESULTS_DIR
then
    echo "Cant make RESULTS_DIR"
    exit
fi

if ! SLOW_FILESYS_REPORT=$(df $SLOW_MOVIE_DIR 2>&1 )
   then
       echo Bad SLOW MOVIE DIR
       echo $SLOW_FILESYS_REPORT
       exit
fi

if [ "$(df $SLOW_MOVIE_DIR)" == "$(df .)" ]
then
    echo "Movie dir $SLOW_MOVIE_DIR and programs $(pwd) are on the same filesystem"
    COPYMOVIES=
else
    COPYMOVIES=true
    FAST_MOVIE_DIR="$(pwd)/TMPMOVIEDIR"
    mkdir -p $FAST_MOVIE_DIR
fi

numMovieFiles=`ls $movie_files | wc -l`

echo "We will process this (these) $numMovieFiles movie file(s)."

#echo $movie_files
ls $movie_files
echo
o=0
for movie_file in $movie_files
do
    ((++o))
    echo "Begin processing $o of $numMovieFiles movie file(s)"
    
    if [ $COPYMOVIES ]
    then
	moviePrefix=`basename ${movie_file%.$ext}`
	echo "Checking if we already have the copy"
	if diff $movie_file $FAST_MOVIE_DIR/$moviePrefix.$ext
	then
	    echo 'They are the same'
	else
	    echo 'Copying (rsync) one movie from slow to fast filesystem.'
	    rsync -pH --progress $movie_file $FAST_MOVIE_DIR 
	    if [ $? != 0 ]
	    then
		echo 'Copying (rsync) failed.  exiting.'
	    fi
	fi
	
	movie_file=$FAST_MOVIE_DIR/$moviePrefix.$ext
	FAST_MOVIE_DELETE_ME=$movie_file
    else
	moviePrefix=`basename ${movie_file%.$ext}`
    fi
 
    echo "Making Mick West redundant, processing ONI FOIA files..."
    FileName="${moviePrefix}.${ext}"
    echo $FileName

    #Save as a results the literal commands we gave, so one can
    #rerun, debug, etc!
    COMMAND_ARCHIVE_PATHNAME=${RESULTS_DIR}/${moviePrefix}.cmds
    cat /dev/null > ${COMMAND_ARCHIVE_PATHNAME}    #So we can just append anytime
    
    echo "Repaying TTSA investors, straightening Uri Gellar's spoons..."
    cd $BITMAPS_DIR  #ffmpeg puts bitmaps in its cwd.
    
    #ffmpeg -threads 0 -hide_banner -an -i $movie_file -vf "scale=trunc(iw/4)*2:trunc(ih/4)*2,decimate,setpts=N/100/TB" -fps_mode vfr thumb%06d.bmp
    
    #
    # EXTRACT or REUSE BMPS
    #
    #
    if [ ${REUSE_BMPS}"" != "yes" ]
    then
	echo "Extracting .bmp's"
	# BITMAPS_DIR is assurred at the beginning, not sep. for each movie
	rm  -f $BITMAPS_DIR/*
	#aside from saving space, we must delete old bitmaps
	#because old files are not necessarilly written over
	# by the C++ programs.  -f makes succeed even if dir bitmaps doesn't exist.
	# Don't use -r since it's better not remake a dir because
	# we can then watch it from another
	# shell.  (In unix, a dir/file remade with the same name is different.)
	
	#ffmpeg -xerror -threads 0 -hide_banner -an -i ${movie_file}  thumb%06d.bmp  &> ${RESULTS_DIR}/ffmpeg.outputs 

	#ok diff with brother HalfDecimated
	FFMPEG_EXTRACT_CMD="ffmpeg -xerror -threads 0 -hide_banner -an    \
	-i $movie_file                                                    \
	thumb%06d.bmp                                                     \
	&> $RESULTS_DIR/ffmpeg.outputs"
	
	movie_width=$(widthOfMovie $movie_file)
	movie_height=$(heightOfMovie $movie_file)

	##scaling? We actually do half-scaling and full scaling.
	#OK diff with brother HalfDecimated
	#"ffmpeg -xerror -threads 0 -hide_banner -an  \
        #   -i $movie_file                                                 \
        #   -vf                                                            \
        #    'scale=trunc(iw/4)*2:trunc(ih/4)*2,decimate,setpts=N/100/TB'  \
        #   thumb%06d.bmp                                                  \
        #   &> $RESULTS_DIR/ffmpeg.outputs"

	  echo Running
	  echo ${FFMPEG_EXTRACT_CMD}
	  ( echo  ; echo ${FFMPEG_EXTRACT_CMD} ) | cat >>${COMMAND_ARCHIVE_PATHNAME}

	  xterm -geometry 160x30+0+180 -title 'ffmpeg extract bitmaps'  -e tail -f ${RESULTS_DIR}/ffmpeg.outputs &
	  Extract_xterm_PID=$!  #so we can kill you later.
	  
	  eval ${FFMPEG_EXTRACT_CMD}
    
    if [ ! $? ]
    then
	echo
	echo ffmpeg error
	echo opening emacs on ffmpeg output
	emacs ${RESULTS_DIR}/ffmpeg.outputs &
	echo exiting
	exit
    fi
    echo 
    echo "ffmpeg done."
    echo
    if ! depthOfBmpIs24 thumb000001.bmp
    then
	echo 'thumb000001.bmp does not exist (in its proper place)'
	echo or its non-24bit depth is not supported by Phase1a
    else
	bmp_width=$(widthOfBmp thumb000001.bmp)
	bmp_height=$(heightOfBmp thumb000001.bmp)
    fi
    #done with extrating bitmaps.
    else
	echo
	echo "We're reusing movie bitmaps for debugging speed."
	echo
	rm -f pic*.bmp #only delete images used to make the previous "baby movie"
	if ! depthOfBmpIs24 thumb000001.bmp
	then
	    echo 'thumb000001.bmp does not exist (in its proper place)'
	    echo or its non-24bit depth is not supported by Phase1a
	else
	    bmp_width=$(widthOfBmp thumb000001.bmp)
	    bmp_height=$(heightOfBmp thumb000001.bmp)
	fi
    fi
    
    #
    # ffmpeg usage explanation for command ABOVE
    #
    # &> redirects both stdout and stderr to save temporarilly.
    # because the FLIRanalysisPhase1aCamX.cpp processes all files in the cwd.
    #
    # --from ffmpeg documentation:
    # -fps_mode vfr (failed with sdc's old 4.4 ffmpeg version, so he removed it)
    #  "Frames are passed through with their timestamp or
    #   dropped so as to prevent 2 frames from having the same timestamp."
    #
    # in the filter (quoted string)
    #  decimate = "Drop duplicated frames at regular intervals."
    #  setpts changes the presentation timestamp (pts)
    #    N is the input frame count
    #    TB is the Time Base of the input timestamps (usually 1/framerate)
    #

    nframes=`ls  | wc -l`
    echo "$nframes frames captured"

    #echo ${moviePrefix}
    #This is the input movie name without movie type extension.
    #Result files will be named
    # ${moviePrefix}.int
    # ${moviePrefix}.out
    # ${moviePrefix}.MOV (that's the "baby movie")

    echo
    echo Input Origin Report:
    if [ ${movie_width}"" = "" ]
    then
	echo 'We are reusing thumb[0-9]^6.bmp-s somehow previously extracted.'
	echo 'bmp_width='${bmp_width} 'bmp_height='${bmp_height}
    else
	echo 'We extracted bitmaps from the movie:'
	echo $movie_file
	echo 'movie_width='$movie_width 'movie_height='$movie_height
	echo 'And, FYI,:'
	echo 'bmp_width='$bmp_width 'bmp_height='$bmp_height
    fi

    if [ $bmp_width"" != "1920" ]
    then
	echo Until a new version is completed,
	echo clipping is done in Phase1a, maybe other stuff is done in Phase1b,
	echo based on 1920x1080 bmps, not what you have.
    else
	if [ $bmp_height"" != "1080" ]
	then
	    echo Until a new version is completed,
	    echo clipping is done in Phase1a, maybe other stuff is done in Phase1b,
	    echo based on 1920x1080 bmps, not what you have.
	    echo Hmm you have 1920 width, but bmp_height=${bmp_height}
	fi
    fi

    echo
    echo "Beginning Phase1a"
    echo

    RESULT_OF_1a_BASE="${moviePrefix}.int"
    cat /dev/null > ${RESULTS_DIR}/${RESULT_OF_1a_BASE}
    #Phase1a used to be will run multiple times, appending each time,
    # (but the script now only does one run.)
    # If we did staged runs, we must start with nothing.
    # Also, this ensures xterm's tail doesn't fail.

    xterm -geometry 180x30+0+180 -title 'Phase 1a (.int file) output'  -e tail -f ${RESULTS_DIR}/${RESULT_OF_1a_BASE} -s 0.1 &
    Phase1a_xterm_PID=$!  #so we can kill you later.
    
    cd $BITMAPS_PARENT_DIR
    #That's where the C++ image processors expect us to be
 
    if [ true ]
    then
	# run Phase1a once on all the frames
	Phase1a_cmd_args="${SOFTWARE_DIR}/$Phase1a 0 $nframes 0"
	#OK diff with brother HalfDecimated
	Phase1a_cmd_args="$Phase1a_cmd_args ${NEW_bitmaps} ${NEW_CamSett} --no-crop --camera-index 1 --pix-scale 2"
	Phase1a_cmd="${Phase1a_cmd_args} > ${RESULTS_DIR}/${RESULT_OF_1a_BASE}" 
	echo Running
	echo ${Phase1a_cmd}
	( echo ; echo ${Phase1a_cmd} ) | cat >>${COMMAND_ARCHIVE_PATHNAME}

	#HUH? commanding ${Phase1a_cmd} makes some shell fail to redirect stdout!
	eval ${Phase1a_cmd}
	#${Phase1a_cmd_args} > ${RESULTS_DIR}/${RESULT_OF_1a_BASE}
	err=$?
	if [ ${err} != 0 ]
	then
	    echo "Phase1a run in a single stage returned error code $err"
	    echo "$0 running in cwd=" $(pwd)
	    echo "$0 will exit. Heres the bad command:"
	    echo
	    echo ${Phase1a_cmd}
	    echo
	    exit 1
	fi

	
	#first 0 = first frame number
	#2nd 0 = CC (Cloud Cover) parameter; not used.
    else
	#
	# orig. code runs Phase1 in stages
	# we must consult Matt Sz. to see if we should get rid of this.
	# One frame pair comparison is lost for each new stage.
	# 
	u=0; v=0
	w=30
	t=$((f/w))
	echo "Removing any quantum woo; reticulating $t splines..."
	#since the stager appends to the result file..
	cat /dev/null > ${RESULTS_DIR}/${RESULT_OF_1a_BASE}
	while [ $u -lt $nframes ]
	do

	    Phase1a_cmd_args="${SOFTWARE_DIR}/$Phase1a $u $t 0 "
	    #OK diff with brother HalfDecimated
	    Phase1a_cmd_args=" ${Phase1a_cmd_args} ${NEW_bitmaps} ${NEW_CamSett} --no-crop --camera-index 1 --pix-scale 2"
	    Phase1a_cmd="${Phase1a_cmd_args} >> ${RESULTS_DIR}/${RESULT_OF_1a_BASE}"
	    #HUH? commanding ${Phase1a_cmd} makes some shell fail to redirect stdout!
	    #${Phase1a_cmd_args} >> ${RESULTS_DIR}/${RESULT_OF_1a_BASE}
	    eval ${Phase1a_cmd}  #this does the trick
	    err=$?
	    if [ ${err} != 0 ]
	    then
		echo "Phase1a in loop of stages returned error code $err"
		echo "$0 running in cwd=" $(pwd)
		echo "$0 will exit. Heres the bad command:"
		echo
		echo ${Phase1a_cmd}
	    fi

	    ((++v))
	    echo Phase 1A: From $u to $((t+u)) "," Part $v of $w Done
	    if [ $v -gt $w ]
	    then
		echo "Don't panic -- there was a remainder from the division!"
	    fi
	    u=$((u+t))
	    if [ $((u+t)) -gt $nframes ]
	    then
		t=$((nframes-u))
	    fi
	done
    fi

    ndiffs=$(cat ${RESULTS_DIR}/${RESULT_OF_1a_BASE} | wc -l )
    echo
    echo Phase1a computed $(cat ${RESULTS_DIR}/${RESULT_OF_1a_BASE} | wc -l ) difference lines "in"
    echo "${RESULTS_DIR}/${RESULT_OF_1a_BASE}"

    echo
    echo Beginning Phase1b
    echo
    
    RESULT_OF_1b_BASE="${moviePrefix}.out"
    cat /dev/null > ${RESULTS_DIR}/${RESULT_OF_1b_BASE}
    
    #so xterm's less doesnt fess. We use less since the result is finished fast.
    xterm -geometry 80x80+0+0 -sb -title 'Phase1b .out' -e less -f ${RESULTS_DIR}/${RESULT_OF_1b_BASE} &
    Phase1b_xterm_PID=$!

    #
    # level for Phase1b first try is 0.5, here----------------------------------------------V---
    #
    Phase1b_cmd_args="${SOFTWARE_DIR}/$Phase1b ${RESULTS_DIR}/${RESULT_OF_1a_BASE} $ndiffs 0.5 "
    Phase1b_cmd_args="${Phase1b_cmd_args} ${NEW_CamSett}"
    Phase1b_cmd="${Phase1b_cmd_args} > ${RESULTS_DIR}/${RESULT_OF_1b_BASE}"

    echo Running
    echo ${Phase1b_cmd}
    ( echo ; echo ${Phase1b_cmd} ) | cat >>${COMMAND_ARCHIVE_PATHNAME}
    #HUH? commanding ${Phase1b_cmd} makes some shell fail to redirect stdout!
    #${Phase1b_cmd_args} > ${RESULT_DIR}/${RESULT_OF_1b_BASE}
    
    MESSAGES_FROM_1b_BASE="${moviePrefix}.mess"
    MESS_1b_FULL_PATH=${RESULTS_DIR}/${MESSAGES_FROM_1b_BASE}
    echo "First 1b try:" | cat > $MESS_1b_FULL_PATH

    xterm -geometry 180x30+50+180 -title 'Phase 1b messages'  -e less -f $MESS_1b_FULL_PATH &
    Phase1b_mess_xterm_PID=$!  #so we can kill you later.

    echo ${Phase1b_cmd} | cat >> $MESS_1b_FULL_PATH
    eval ${Phase1b_cmd} 2>> $MESS_1b_FULL_PATH
    err=$?
    if [ ${err} != 0 ]
    then
	echo "Phase1b run returned error code $err"
	echo "$0 running in cwd=" $(pwd)
	#echo "$0 will continue and do a redo with level=0.98 instead of 0.5"
	echo "$0 will exit.  See cmd above^^."
	exit 1
    fi
    n=`cat ${RESULTS_DIR}/${RESULT_OF_1b_BASE} | wc -l`
    echo
    echo "First try of Phase1b reported $n frames have objects, see the lines in:"
    echo ${RESULTS_DIR}/${RESULT_OF_1b_BASE}
    echo
    
    if [ $n -gt 50000 ] || [ $n -eq 0 ]
    then
	if [ $n == 0 ]
	then
	    echo "Part deux: electric boogaloo... First try found 0 frames with objects."

	    #we don't redo Phase1a since current version ignores cloud cover param."
#	    echo "We will redo Phase1a"
#
#	    u=0; v=0
#	    w=30
#           t=$((nframes/w))
#           while [ $u -lt $nframes ]
#            do
#	        #not updated below
#		${SOFTWARE_DIR}/$Phase1a $u $t 1 >> ${RESULTS_DIR}/${RESULT_OF_1a_BASE} 2> /dev/null 
#		((++v))
#		echo Phase 1Bs 1A redo: Part $v of $w Done
#		if [ $v -gt $w ]
#		then
#                   echo "Don't panic -- there was a remainder from the division!"
#		u=$((u+t))
#		if [ $((u+t)) -gt $nframes ]
#		then
#                   t=$((nframes-u))
#		fi
#            done
#


	    #
	    # level for Phase1b redo is 0.98, here-----------------------------------------------------------V---
	    #
	    Phase1b_redo_command_args="${SOFTWARE_DIR}/$Phase1b ${RESULTS_DIR}/${RESULT_OF_1a_BASE} $ndiffs 0.98 "
	    Phase1b_redo_command_args="${Phase1b_redo_command} ${NEW_CamSett}"
	    Phase1b_redo_command="${Phase1b_redo_command_args} > ${RESULTS_DIR}/${RESULT_OF_1b_BASE}"
	    ( echo  ; echo "${Phase1b_redo_command}" ) | cat >> ${COMMAND_ARCHIVE_PATHNAME}
	    echo "${Phase1b_redo_command}" | cat >> $MESS_1b_FULL_PATH
	    eval $Phase1b_redo_command 2>> $MESS_1b_FULL_PATH
	    err=$?
	    if [ ${err} != 0]
	    then
		echo "Phase1b redo run returned error code $err"
		echo "$0 running in cwd=" $(pwd)
		#echo "$0 will continue and do a redo with level=0.98 instead of 0.5"
		echo "$0 will exit.  See cmd above^^."
		exit 1
	    fi


	    n=`cat ${RESULTS_DIR}/${RESULT_OF_1b_BASE} | wc -l`
	    echo "Phase 1B: Re-Done reports $n frames with objects."

	fi
    fi
    
    #now, after either one or a redo of Phase1b,
    # don't waste time on empty file OR overloading your HD
    if [ $n -gt 465000 ] || [ $n -eq 0 ]
    then
	echo "Either too many or too few objects..."
	echo "On to the next movie, if any"
	continue #top level movie loop
    fi
    # number above assumes 2.9 MB/image: (2.9*(46500+216e3))/1e3 = 760 GB of free space needed! Adjust for your machine


    cd ${BITMAPS_PARENT_DIR}
    
    echo "Logging range, deploying Little Green Men..circling 'em in picnn.bmps"
    
    ##scaling? x and y are pixel coords.
    cat ${RESULTS_DIR}/${RESULT_OF_1b_BASE} | while read evt frame extr x y prob
    do
	((++frame))
	#if (( $frame %5 == 0 ))
	#then
	    #echo "processing frame number" $frame
	#fi
	i=$((x-9))  ##scaling?  maybe 9 could remain unscaled
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
	    #Remember, we're in ${BITMAPS_PARENT_DIR}
	    #scaling?  Convert command works in terms of pixel coords, it must.
	    BITMAP_EDIT_CMD=\
"convert ${BITMAPS_DIR_NAME}/thumb$pad$frame.bmp -fill none -stroke cyan -draw 'circle $i,$j $k,$l' ${BITMAPS_DIR_NAME}/pic$pad$frame.bmp"
	else
	    # echo  '$y -lt 800 no branch'
	    BITMAP_EDIT_CMD=\
"convert ${BITMAPS_DIR_NAME}/thumb$pad$frame.bmp -fill none -stroke lime -draw 'circle $i,$j $k,$l' ${BITMAPS_DIR_NAME}/pic$pad$frame.bmp"
	fi
	# ..echo 'We will eval' $BITMAP_EDIT_CMD
	eval $BITMAP_EDIT_CMD
    done
    
    echo "Breaking Lue's NDA...make a movie of them"
    cd ${BITMAPS_DIR}
    #We just had added the pic*.bmp frames for the 'baby movie'
    EXT="MOV" #NOT same as ext above!
    #mogrify -resize 640x360 "*" *.bmp #mogrify -format jpg *.bmp
    echo "ffmpeg now makes a baby movie." | cat >> ${RESULTS_DIR}/ffmpeg.outputs
    ffmpeg -hide_banner -y -threads 0 -r 60 -f image2 -pattern_type glob -i 'pic*.bmp' -vcodec libx264 -crf 25 -pix_fmt yuv420p ${RESULTS_DIR}/${moviePrefix}.${EXT} &>> $RESULTS_DIR/ffmpeg.outputs 

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
    
    # We're done with I/O thru the bitmaps dir
    cd ${SOFTWARE_DIR}

    # save the result movie
    
    
    #echo "Cleaning detritus..." #I'd rather leave that for the build system.

    #rm -rf *.cpp~ *.sh~ *.txt~ *.err bitmaps/
    # sdc: Keep old bitmaps BUT the script removes them early next time
    # because the C++ programs process all files in their cwd.

    #rm -f *.cpp~ *.sh~ *.txt~ 

    if [ $COPYMOVIES ]
    then
	echo "about to delete input movie"
	echo $movie_file
	rm $movie_file
    fi
    
    echo "Cosmic consciousness has been achieved!!"
    echo "from yet another movie $o of $numMovieFiles."

done

exit_greeting

# buy a Mac or install Linux. Just kidding (But get Ubuntu for Windows!)
# remove all instances of -Ofast (or replace w/ -O3). There should be 4
# Change w=30 to higher nums (go into hundreds or even 1000 if need be)
# to avoid 'Killed' messages. Just make sure still divisible by 216,000
# If FFMPEG is slow i.e. speed=2x instead of ~7-8x from Mac
# change "-threads 0" by 1 until max possible of 16 (a multi-core comp)
# Zero is the default-but doesn't seem to optimize properly on Windows
# Most ideally you run this software on a machine which possesses GPUs!
# superuser.com/questions/1600352/hardware-acceleration-ffmpeg-on-windows
