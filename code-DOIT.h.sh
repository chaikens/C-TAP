echo SOURCED: c-TAP-DOIT.h.sh

#Phase 1a,b options additions (value is --option optvalue, etc)
#use opt_phase1a_bitmaps
#use OPT_CamSett

cd ${SOFTWARE_DIR}

###################################
#
Phase1a=Phase1aPipeOpt  #compiled from FLIRanalysisPhase1aCamXPipeOpt.cpp, alongside us.
Phase1b=Phase1bPipeOpt  #compiled from FLIRanalysisPhase1bCamXPipeOpt.cpp, alongside us.
#
#####################
if ! make ${Phase1a} ${Phase1b}
then
    echo make the C++ progs we need failed.  Check this out.
    exit 1
fi

if [ $ARCHITECTURE = "framefile" ]
then
    
    if ! mkdir -p $BITMAPS_DIR
    then
	echo "Cant make BITMAPS_DIR $BITMAPS_DIR"
	exit 1
    fi
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

    ###########################################################################
    source ${SOFTWARE_DIR}/code-ONEMOVIE.h.sh
    ###########################################################################
    
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




