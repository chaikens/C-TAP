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

FAST_FILESYS_DIR_IF_USED="/media/seth/CTAP"

PHASE_1a=F1a.all
PHASE_1a_RESULT_OLD_N_OR_NONE=


ARCHITECTURE="framefile"
#"framefile" #vs pipeline

if [ $ARCHITECTURE = "framefile" ]
then		   
       REUSE_BMPS=no
       BITMAPS_DIR_NAME="bitmaps-${JOBNAME}" 
       BITMAPS_PARENT_DIR=$FAST_FILESYS_DIR_IF_USED/bmdir 
       BITMAPS_DIR="${BITMAPS_PARENT_DIR}/${BITMAPS_DIR_NAME}"
fi

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

movie_files="$SLOW_MOVIE_DIR/DS.$ext"

#################################
source code-MOVIES.h.sh         #
#################################
echo YOUR-ANALYSIS-$JOBNAME     #
#################################

#I think the way, to start, to vary subsequent phases of a job is to
#override the default from code-HELPERS-DEFAULTS
#    RESULTS_DIR=${SOFTWARE_DIR}/RESULTS-${JOBNAME}-$(mydate)
#  but the job is named from the C-TAP....sh file..??


############ Vary CamSett.txt settings here! #########
cat > tempCamSett.txt <<EOF
smallestThr= 0
biggestThr= 254
smallestPix= 0
biggestPix= 67
SkewGaussAmpl= 0.633
SkewGaussXi= 1.97
SkewGaussOmega= 1.89
SkewGaussAlpha= 2.5
NumPixAbvThrSumMin= 0
NumPixAbvThrSumMax= 3
SubThr= 20
RewFram= 0
ForFram= -2
FramBefNew= 1
FracYes= 1.0
CROP_XI= 0
CROP_XF= 500
CROP_YI= 0
CROP_YF= 1919
mainThreshold= 14
EOF

PHASE_1b_LEVEL=0.5
PHASE_1b_LEVEL_REDO=0.98

OPT_CamSett="--CamSett-file $(pwd)/tempCamSett.txt"

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
#Also, the 2nd filter setpts is set presentation timestamps
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
#     OTHER_OPTIONS=" --camera-index 1"
#     #In the Phase1a C++ program, we wrote an exclusion zone function for DroneShort1,
#     #and also named a camera for it.

#for HALF FRAME, (DECIMATED) extraction.

MOVIE_TO_FRAME_DIV=2 #Should get rid of this.
MOVIE_SCALE_OPTION="--movie-scale 2"
PIXPROC_SCALE_OPTION="--pixproc-scale 1"
USER_SCALE_OPTION="--user-scale 1"
OTHER_OPTIONS=" --camera-index 1"

#In the Phase1a C++ program, we wrote
#an exclusion zone function for DroneShort1,
#and also named a camera for it.  

##### Explain your run into the log at top and for each movie###
RUN_EXPLANATION="ffmpeg .mp4---->Half len/wid decimated frame (ffmpeg filter) .yuv raw (our table based YUVtoBMP pipeline)----->.bmp sequence -----> (Phase1aPipe) (redirects to/from files) > movie.int > (Phase1bPipe) > file.out  This is coded by $0"
echo $RUN_EXPLANATION

####################################
source code-ANALYSIS.h.sh          #
source code-DOIT.h.sh              #
####################################
