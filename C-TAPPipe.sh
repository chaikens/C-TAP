#!/bin/bash
#
# manual settings
#
#settings for debugging or not.
DEBUG=true
#DEBUG=false
#Also, set debugging or not in our Makefile

#
# Please code below where you would like us to
# find 1 or more movies to process, plus
# one name or a pattern for them all.
#
# We test on our single, short movie by
# putting it (or a symlink) in the same
# dir as the script.
#
#######################################################
#    CODE HERE WHERE YOUR MOVIES ARE                  #
#  and their FILENAME EXTENSION (also see below)      #
#                                                     #
SLOW_MOVIE_DIR=$(pwd)
#                                                     #
ext=mp4
#It must be standard and correct for ffmpeg to use.   #
#######################################################

# One of our functions is to copy (using rsync)
# movies from a slow filesystem to the one containing the script,
# presumedly a faster one.
# The script detects if the filesystems are the same and
# in that case, skips the copying.  So, when we're out
# of the box (and you put DroneShort1.mp4 alongside the
# script, copying is not done.
#
# Example where we use SLOW_MOVIE_DIR to specify
#the dir (say on a slow external drive) that contains
#a family of movies, say taken by one session of camera shooting.
#
# Whence (after USB hotplugging),
#/data/GIT/C-TAP$ ls /media/seth/CTAP/2025-01-17/
# N884A6_ch1_main_20250117000000_20250117010000.mp4
# N884A6_ch1_main_20250117040000_20250117050000.mp4  
# N884A6_ch1_main_20250117200000_20250117210000.mp4
#
# We code the shell globbing pattern in quotes:
# SLOW_MOVIE_DIR=/media/seth/CTAP/2025-01-17/
# ext=mp4
# movie_files="SLOW_MOVIE_DIR/N884A6_ch1_main_*.${ext}"
#
# Some movie file names incorporate the camera name, so we can select
# from one camera, eg ..$path/*_A1.
#
#This script is pre-programmed to test/demonstrate on
#125MB test movie DroneShort1.mp4.  See README.md for getting a copy.
#It results in a clear true
#positive case detection of a purposely flown drone in daytime.
#
#
##############################################################
#   SPECIFY YOUR MOVIE FILES HERE FOR THE SCRIPT TO PROCESS  #
#   THEM ALL, RUNNING THE ANALYSES ONE AT A TIME FOR EACH,   #
#   and putting the RESULTS in SEPARATE FILES.               #
#                                                            #
movie_files="$SLOW_MOVIE_DIR/DroneShort1.$ext"  
#                                                            #
##############################################################

# CWD we should be in when calling our programs from this script
SOFTWARE_DIR=$(pwd)

# where our results are written
RESULTS_DIR="`pwd`/RESULTS"

# RESULTS:
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


# (from original comments, edited)
# PRE-REQ: FFMPEG (any v) +ImageMagick (Win: command prompt not enough)
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

if [ $DEBUG = "true" ]
then
    echo
    echo We are debugging
    echo
    ulimit -c unlimited
    MAKE_DEBUG=yes #not used yet
    #In pipeline versions, there are no Phase1a disk file
    #.bmps or other saved frame images.
    #I plan to pipeline that after Phase1b, frames
    # are selected circles drawn into them, and
    # they are combined by ffmpeg to make the baby movie.
    #
    #Manually deal with this saving for now,
    # when saving is actually done for regression and
    # old version conparisons.
    REUSE_BMPS=no
else
    ulimit -c 0
    MAKE_DEBUG=no  #not used yet Remember to comment earlier
    #that optimized compiling should be used except for debugging.
    # script can do make ...  DEBUGGING=false or
    #  make ...  CXXFLAGS=-03 (overriding -g )
    # But we like -Wno-unused-variables to stop warnings make will convey
    # when we use -O3  ... We should use a custom control like DEBUGGING
    # rather than override I think.
fi

################################################################
#                                                              #
#  END OF REGION FOR USER CONFIGURATION EDITS                  #
#                                                              #
################################################################

cd $SOFTWARE_DIR

# our 2 programs the script calls.
# We've retained their original source file names, the exe names
# below come from our Makefile.
Phase1a=Phase1a  #compiled from FLIRanalysisPhase1aCamX.cpp, alongside us.
make Phase1a
Phase1b=Phase1b  #compiled from FLIRanalysisPhase1bCamX.cpp, alongside us.
make Phase1b

# v1.0 uses ./bitmaps for .bmps
# (1) extracted from movies
# (2) created by this script when making the "baby movie"

BITMAPS_PARENT_DIR=$(pwd)
BITMAPS_DIR="$BITMAPS_PARENT_DIR/bitmaps"
if ! mkdir -p $BITMAPS_DIR
then
    echo "Cant make BITMAPS_DIR $BITMAPS_DIR"
    exit
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

echo "We will process these $numMovieFiles movie files."

#echo $movie_files
ls $movie_files
echo
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
	echo '(debugging)'
	echo "our movie_file:"
	echo ${movie_file}
    fi
 
    echo "Making Mick West redundant, processing ONI FOIA files..."
    FileName="${moviePrefix}.${ext}"
    echo $FileName


    
    echo "Repaying TTSA investors, straightening Uri Gellar's spoons..."
    cd $BITMAPS_DIR  #ffmpeg puts bitmaps in its cwd.

    echo "Calling ffmpeg to extract bitmap frames..to watch this, run in another window:"
    echo "tail -f $RESULTS_DIR/ffmpeg.outputs"
    echo
    #ffmpeg -threads 0 -hide_banner -an -i $movie_file -vf "scale=trunc(iw/4)*2:trunc(ih/4)*2,decimate,setpts=N/100/TB" -fps_mode vfr thumb%06d.bmp
    
    # try below doesn't work.
    #touch $RESULTS_DIR/ffmpeg.outputs
    #xterm -e tail -f $RESULTS_DIR/ffmpeg.outputs

    if [ $REUSE_BMPS != "yes" ]
    then
	# BITMAPS_DIR is assurred at the beginning, not sep. for each movie
	rm  -f $BITMAPS_DIR/*
	#aside from saving space, we must delete old bitmaps
	#because old files are not necessarilly written over
	# by the C++ programs.  -f makes succeed even if dir bitmaps doesn't exist.
	# Don't use -r since it's better not remake a dir because
	# we can then watch it from another
	# shell.  (In unix, a dir/file remade with the same name is different.)
	ffmpeg -xerror -threads 0 -hide_banner -an                          \
           -i $movie_file                                               \
           -vf \
	    "scale=trunc(iw/4)*2:trunc(ih/4)*2,decimate,setpts=N/100/TB" \
	   thumb%06d.bmp \
	   &> $RESULTS_DIR/ffmpeg.outputs 
    else
	echo
	echo "We're reusing movie bitmaps for debugging speed."
	echo
	rm -f pic*.bmp #only delete images used to make the previous "baby movie"
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
    echo "Beginning Phase1a"
    echo

    RESULT_OF_1a_BASE="${moviePrefix}.int"
    rm -f ${RESULTS_DIR}/${RESULT_OF_1a_BASE}
    #Phase1a used to be will run multiple times, appending each time,
    # (but the script now only does one run.)
    # If we did staged runs, we must start with nothing.

    cd $BITMAPS_PARENT_DIR
    #That's where the C++ image processors expect us to be
    # for now "bitmaps/thumb{0-9}^6.bmp" filenames are hard coded. 

    if [ true ]
    then
	# run Phase1 once on all the frames
	./$Phase1a 0 $nframes 0 > ${RESULTS_DIR}/${RESULT_OF_1a_BASE}
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
	while [ $u -lt $nframes ]
	do
	    ./$Phase1a $u $t 0 >> ${RESULTS_DIR}/${RESULT_OF_1a_BASE}
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
    echo Phase1 Completed $(cat ${RESULTS_DIR}/${RESULT_OF_1a_BASE} | wc -l ) difference lines \
    "in ${RESULTS_DIR}/${RESULT_OF_1a_BASE}"

    echo
    echo Beginning Phase1b
    echo
    
    RESULT_OF_1b_BASE="${moviePrefix}.out"
    rm -f ${RESULTS_DIR}/${RESULT_OF_1b_BASE}
    
    echo "./$Phase1b ${RESULTS_DIR}/${RESULT_OF_1a_BASE} $ndiffs 0.5 > ${RESULTS_DIR}/${RESULT_OF_1b_BASE}"
    echo
    
    if ./$Phase1b ${RESULTS_DIR}/${RESULT_OF_1a_BASE} $ndiffs 0.5 > ${RESULTS_DIR}/${RESULT_OF_1b_BASE}
    then
	n=`cat ${RESULTS_DIR}/${RESULT_OF_1b_BASE} | wc -l`	
	echo "First try of Phase1b reported $n frames have objects."
    else
	echo "First Phase1b failure"
	exit
    fi
    
    if [ $n -gt 50000 ] || [ $n -eq 0 ]
    then
	if [ $n == 0 ]
	then
	    echo "Part deux: electric boogaloo... First try found 0 frames with objects."
	    echo "We will redo Phase1a"
	    u=0; v=0
	    w=30
            t=$((nframes/w))
            while [ $u -lt $nframes ]
            do
		./$Phase1a $u $t 1 >> ${RESULTS_DIR}/${RESULT_OF_1a_BASE} 2> /dev/null 
		((++v))
		echo Phase 1Bs 1A redo: Part $v of $w Done
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
	#rm $Output
	if ./$Phase1b ${RESULTS_DIR}/${RESULT_OF_1a_BASE} $ndiffs 0.98 > ${RESULTS_DIR}/${RESULT_OF_1b_BASE}
	   then
	       n=`cat ${RESULTS_DIR}/${RESULT_OF_1b_BASE} | wc -l`
	       echo "Phase 1B: Re-Done reports $n frames with objects."

	fi
    fi
    
    # don't waste time on empty file OR overloading your HD
    if [ $n -gt 465000 ] || [ $n -eq 0 ]
    then
	echo "Either too many or too few objects..."
	echo "On to the next movie, if any"
	continue #top level movie loop
    fi
    # number above assumes 2.9 MB/image: (2.9*(46500+216e3))/1e3 = 760 GB of free space needed! Adjust for your machine
    
    echo "Logging range, deploying Little Green Men.."
    cat ${RESULTS_DIR}/${RESULT_OF_1b_BASE} | while read evt frame extr x y prob
    do
	((++frame))
	if (( $frame %5 == 0 ))
	then
	    echo "processing frame number" $frame
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
"convert bitmaps/thumb$pad$frame.bmp -fill none -stroke cyan -draw 'circle $i,$j $k,$l' bitmaps/pic$pad$frame.bmp"
	else
	    # echo  '$y -lt 800 no branch'
	    BITMAP_EDIT_CMD=\
"convert bitmaps/thumb$pad$frame.bmp -fill none -stroke lime -draw 'circle $i,$j $k,$l' bitmaps/pic$pad$frame.bmp"
	fi
	# ..echo 'We will eval' $BITMAP_EDIT_CMD
	eval $BITMAP_EDIT_CMD
    done
    
    echo "Breaking Lue's NDA..."
    cd bitmaps/
    EXT="MOV" #NOT same as ext above!
    #mogrify -resize 640x360 "*" *.bmp #mogrify -format jpg *.bmp
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
    
    #ffmpeg -i ${moviePrefix}.${ext} -filter:v "transpose=1,transpose=1" flipped.mp4
    
    # We're done with I/O thru the bitmaps dir
    cd ..

    # save the result movie

    echo "Cleaning detritus..."

    #rm -rf *.cpp~ *.sh~ *.txt~ *.err bitmaps/
    # sdc: Keep old bitmaps BUT the script removes them early next time
    # because the C++ programs process all files in their cwd.

    rm -f *.cpp~ *.sh~ *.txt~ 

    if [ $COPYMOVIES ]
    then
	echo "about to delete input movie"
	echo $movie_file
	rm $movie_file
    fi
    
    echo "Cosmic consciousness has been achieved!!"
    echo "from yet another movie $o of $numMovieFiles."

done

echo "True, full consciousness can now be reasserted by you poor human user."

# buy a Mac or install Linux. Just kidding (But get Ubuntu for Windows!)
# remove all instances of -Ofast (or replace w/ -O3). There should be 4
# Change w=30 to higher nums (go into hundreds or even 1000 if need be)
# to avoid 'Killed' messages. Just make sure still divisible by 216,000
# If FFMPEG is slow i.e. speed=2x instead of ~7-8x from Mac
# change "-threads 0" by 1 until max possible of 16 (a multi-core comp)
# Zero is the default-but doesn't seem to optimize properly on Windows
# Most ideally you run this software on a machine which possesses GPUs!
# superuser.com/questions/1600352/hardware-acceleration-ffmpeg-on-windows
