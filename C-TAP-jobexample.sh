#!/usr/bin/bash
#              (hmm---no comment can follow bash above)#
#FOR STANDARD USAGE, EDIT ONLY      #
#THE FILE-NAME OF THIS SCRIPT       #
#AND CODE OUTSIDE BOXES LIKE THIS.  #
#THE FILE-NAME SHOULD HAVE THE FORM #
#  C-TAP-<jobname>.sh               #
source code-HELPERS-DEFAULTS.h.sh   #
echo Running your job $JOBNAME      #
#####################################

############################
echo YOUR-SYSCONF-$JOBNAME #
############################

#if we reuse bitmaps, make sure here to specify the BITMAPS_DIR_NAME!
REUSE_BMPS=no
FAST_FILESYS_DIR_IF_USED="/media/seth/CTAP"
BITMAPS_DIR_NAME="bitmaps-${JOBNAME}"
BITMAPS_PARENT_DIR=$FAST_FILESYS_DIR_IF_USED
BITMAPS_DIR="${BITMAPS_PARENT_DIR}/${BITMAPS_DIR_NAME}"

###########END OF YOUR-SYSCONF###
source code-SYSCONFIG.h.sh      #
#################################
echo YOUR-MOVIES-$JOBNAME       #
#################################

ext=mp4

#Here is an example of how to configure a directory
# full of movies for C-TAP's research, not testing.
#      movie_files="$SLOW_MOVIE_DIR/N884A6_ch1_main_*.$ext"
#

movie_files="$SLOW_MOVIE_DIR/DroneShort1HalfDecimated.$ext"

#################################
source code-MOVIES.h.sh         #
#################################
echo YOUR-ANALYSIS-$JOBNAME     #
#################################

OPT_CamSett="--CamSett-file $(pwd)/CamSett.txt"

#Not done yet: when we specify pipelining here, and include the below
#analysis options, that is what is done.  We'll look at TESTING/TestPipeOpt code
#to do that.

#The FLIR Algorithm requires the frame sequence to be decimated.
#So, frame times get lost when extracted!

#ffmpeg option notes.  We use
## option -vf (alias -filter:v) <video filter script> 
# Eventually we may want to use -filter_complex

#Beware: ffmpeg option names are -single_dash_underscore_sep.
#Whilst our C++ commands use Unix style --double-dash-dash-sep.

#For FULL FRAME with DECIMATION extraction by ffmpeg
#FFMPEG_EXTRACT_FILTER="-vf decimate,setpts=N/100/TB"

#for HALF-RESOLUTION with DECIMATION extraction.
FFMPEG_EXTRACT_FILTER="-vf scale=trunc(iw/4)*2:trunc(ih/4)*2,decimate,setpts=N/100/TB" 

#See code-ANALYSIS.h.sh for the ffmpeg commands using this (or these) above
# single output options.

#Settings below are used for both the pipeline and frame .bmp architectures

#For FULL FRAME, (DECIMATED) RESOLUTION
# These are used for both pipeline and frame .bmp architectures.
# COMMENTED OUT
#     MOVIE_TO_FRAME_DIV=1  #used by pipe arch, but not yet here.
#     MOVIE_SCALE_OPTION="--movie-scale 2"
#     PIXPROC_SCALE_OPTION="--pixproc-scale 1"
#     USER_SCALE_OPTION="--user-scale 1"
#     OTHER_OPTIONS="--no-crop --camera-index 1"
#     #In the Phase1a C++ program, we wrote an exclusion zone function for DroneShort1,
#     #and also named a camera for it.

#for HALF FRAME, (DECIMATED) extraction.

MOVIE_TO_FRAME_DIV=2 #used by the pipe architecture, but not yet here.
MOVIE_SCALE_OPTION="--movie-scale 1"
PIXPROC_SCALE_OPTION="--pixproc-scale 1"
USER_SCALE_OPTION="--user-scale 1"
OTHER_OPTIONS="--no-crop --camera-index 1"

#In the Phase1a C++ program, we wrote an exclusion zone function for DroneShort1,
#and also named a camera for it.  
     
#We're refactoring this from TESTING/TestPipelineOPT where
#we had after defining the above,
#source DoOneMoviePipeCommon.h.sh

####################################
source code-ANALYSIS.h.sh          #
source code-DOIT.h.sh              #
####################################
