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

movie_files="$SLOW_MOVIE_DIR/DroneShort1FullDecimated.$ext"

#################################
source code-MOVIES.h.sh         #
#################################
echo YOUR-ANALYSIS-$JOBNAME     #
#################################

OPT_CamSett="--CamSett-file $(pwd)/CamSett.txt"

#we have to develop so when we specify pipelining here, and include the below
#analysis options, that is what is done.  We'll look at TESTING/TestPipeOpt code
#to do that.

#WARNING--for now, when bmp frame architecture (not pipeline) is used
#you have to customized the extraction in code-ONEMOVIE.h.sh
#Fix to be developed

#---W H E N--- pipeline is used, we define this shell fn someway like these:

#for FULL FRAME extraction.  The argument is the input movie
#for pipeline The argument is the input movie.
function ffmpeg_pipe_extract() {
    ffmpeg -hide_banner -y -i $1 PIPE.yuv &> ffmpeg.log
}

#for .bmp frames
# ffmpeg_bmp_extract movie_file logfile
ffmpeg_bmp_extract() {
    ffmpeg -hide_banner -y -an  \
	   -i $1                \
	   -vf                  \
	   'decimate,setpts=N/100/TB' \
	   thumb%06d.bmp        \
	   &> $2
} 

# These are used for both pipeline and frame .bmp architectures.
MOVIE_TO_FRAME_DIV=1 #not used
MOVIE_SCALE_OPTION="--movie-scale 2"
PIXPROC_SCALE_OPTION="--pixproc-scale 1"
USER_SCALE_OPTION="--user-scale 1"
OTHER_OPTIONS="--no-crop --camera-index 1"
#In the Phase1a C++ program, we wrote an exclusion zone function for DroneShort1,
#and also named a camera for it.

#for HALF FRAME, DECIMATED extraction.
#for pipeline
#ffmpeg_pipe_extract()
#{
#    ffmpeg  -hide_banner -y -an   \
#	    -i $1                 \
#            -vf                   \
#            "scale=trunc(iw/4)*2:trunc(ih/4)*2,decimate,setpts=N/100/TB" \
#	    PIPE.yuv &> ffmpeg.log
#}

#for .bmp frames
# ffmpeg_bmp_extract movie_file logfile
#ffmpeg_bmp_extract() {
#    ffmpeg -xerror -threads 0 -hide_banner -an    \
#	-i $1                                     \
#	-vf                                       \
#	'scale=trunc(iw/4)*2:trunc(ih/4)*2,decimate,setpts=N/100/TB' \
#	thumb%06d.bmp                             \
#	&> $2
#}

#These are used for both the pipeline and frame .bmp architectures

#    MOVIE_TO_FRAME_DIV=2
#    MOVIE_SCALE_OPTION="--movie-scale 1"
#    PIXPROC_SCALE_OPTION="--pixproc-scale 1"
#    USER_SCALE_OPTION="--user-scale 1"
#    OTHER_OPTIONS="--no-crop --camera-index 1"

#In the Phase1a C++ program, we wrote an exclusion zone function for DroneShort1,
#and also named a camera for it.  
     
#We're refactoring this from TESTING/TestPipelineOPT where
#we had after defining the above,
#source DoOneMoviePipeCommon.h.sh

####################################
source code-ANALYSIS.h.sh          #
source code-DOIT.h.sh              #
####################################
