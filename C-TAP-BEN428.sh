#!/usr/bin/bash
#FOR STANDARD USAGE, EDIT ONLY      #
#THE FILE-NAME OF THIS SCRIPT       #
#AND CODE OUTSIDE BOXES LIKE THIS.  #
#THE FILE-NAME SHOULD HAVE THE FORM #
#  C-TAP-<jobname>.sh               #
source code-HELPERS-DEFAULTS.h.sh   #
echo Running your job $JOBNAME      #
#####################################
echo YOUR-SYSCONF-$JOBNAME          #
#####################################

NEW_BABY_DEVEL=yes


function xterm()
{
    echo
    echo INFO: Not starting xterm log monitor. Do manually with:
    echo '(but you must quote the #colors and (-ied) names)'
    echo xterm $@
    echo
}

TRY_CONVERT_picbmp_TO_picbmp=yes
ALLOW_THRASHING_BABIES=yes

KEEP_BABY_BMPS=no
KILL_OUR_XTERMS_ONE_DONE=yes

FAST_MOVIE_DIR=/dev/shm/TMPMOVIES
FAST_FILESYS_DIR_IF_USED=/data/CTAP/scratch #"/media/seth/TerabyteSandisk/CTAP"
#these are used even for pipeline arch, where
#the BITMAP dir is used for the BABY MOVIE
BITMAPS_PARENT_DIR=$FAST_FILESYS_DIR_IF_USED/bmdir 
BITMAPS_DIR_NAME="bitmaps-${JOBNAME}"
BITMAPS_DIR="${BITMAPS_PARENT_DIR}/${BITMAPS_DIR_NAME}"

PHASE_1a=F1a.orig
PHASE_1a_RESULT_OLD_N_OR_NONE=

ARCHITECTURE="pipeline"
#"framefile" #vs pipeline


###########END OF YOUR-SYSCONF###
source code-SYSCONFIG.h.sh      #
#################################
echo YOUR-MOVIES-$JOBNAME       #
#################################

#####################################################################
#           Configure here which movie(s) to process                #
#####################################################################
ext=mp4  #TO DO--refactor to a CAP_UND style USER_SETTING, not internal-var.

SLOW_MOVIE_DIR=/media/seth/SilverOrig/PP-2025-04-28
#movie_files=$(echo ${SLOW_MOVIE_DIR}/N*main_2025mmdd{04,17,18,19,20,21,22,23}*.mp4)
#------------------------------------------------0428------------------------------
#  HERE YYYYMMDD must be 8 adjacent digits--no hyphens unlike our dir names.
movie_files=$(echo ${SLOW_MOVIE_DIR}/N*.mp4)




#####################################################################
#Here is how to configure a dir full of movies for research, not    #
# testing: movie_files="$SLOW_MOVIE_DIR/N884A6_ch1_main_*.$ext"     #
#####################################################################


#################################
source code-MOVIES.h.sh         #
#################################
echo YOUR-ANALYSIS-$JOBNAME     #
#################################

#OPT_CamSett="--CamSett-file $(pwd)/CamSett.txt"

############ Vary CamSett.txt settings here! #########
#
#Our diff from Camsett.ext is the max y cood clipping
#is adjusted to add speed advantage to the new ground hugging
#clipping we now have.  The exclusion zone is activated by
#setting --camera-index 2


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
CROP_XF= 858
CROP_YI= 0
CROP_YF= 1919
mainThreshold= 14
EOF

OPT_CamSett="--CamSett-file $(pwd)/tempCamSett.txt"

#Not done yet: when we specify pipelining here, and include the below
#analysis options, that is what is done.
#We'll look at TESTING/TestPipeOpt code to do that.

#The FLIR Algorithm requires the frame sequence to be decimated.
#So, frame times get lost when extracted!

#ffmpeg option notes.  We use
## option -vf (alias -filter:v) <video filter script> 
# Eventually we may want to use -filter_complex

#Beware: ffmpeg option names are -single_dash_underscore_sep.
#Whilst our C++ commands use Unix style --double-dash-dash-sep.

#For FULL FRAME with DECIMATION extraction by ffmpeg
#Also, the 2nd filter setpts is set presentation timestamps
#FFMPEG_EXTRACT_FILTER="-vf decimate,setpts=N/100/TB"

#for HALF-RESOLUTION with DECIMATION extraction.
#Also, the 2nd filter setpts is set presentation timestamps
FFMPEG_EXTRACT_FILTER=""
# HPP means we use (P)re-(P)rocessed input:
# 1. ORIGINAL frame numbering __before__ decimation
# 2. scaling
# 3. decimation,
# also recoding from HEVC to AVC (default due to ffmpeg)
# NOT
# "-vf scale=trunc(iw/4)*2:trunc(ih/4)*2,decimate,setpts=N/100/TB" 

#See code-ANALYSIS.h.sh for the ffmpeg commands using this (or these) above
# single output options.

# THIS MOVIE ONLY: In the Phase1a C++ program, we wrote an exclusion
# zone function for DroneShort1, and also named a camera for it.

#Settings below are used for both pipeline and framefile architectures.

#For FULL FRAME, (DECIMATED) RESOLUTION
# COMMENTED OUT
#     MOVIE_TO_FRAME_DIV=1  #used by pipe arch, but not yet here.
#     MOVIE_SCALE_OPTION="--movie-scale 2"
#     PIXPROC_SCALE_OPTION="--pixproc-scale 1"
#     USER_SCALE_OPTION="--user-scale 1"
#     OTHER_OPTIONS="--no-crop --camera-index 1"
#     #In the Phase1a C++ program, we wrote an exclusion zone function for DroneShort1,
#     #and also named a camera for it.

#for HALF FRAME, (DECIMATED) extraction
#PREPROCESSING produced this here

MOVIE_TO_FRAME_DIV=1 #used by the pipe architecture, but not yet here.
MOVIE_SCALE_OPTION="--movie-scale 1"
PIXPROC_SCALE_OPTION="--pixproc-scale 1"
USER_SCALE_OPTION="--user-scale 1"
OTHER_OPTIONS="--camera-index 2"

#In the Phase1a C++ program, we wrote
#an exclusion zone function for DroneShort1,
#and also named a camera for it.  
     
##### Explain your run into the log at top and for each movie###

RUN_EXPLANATION="Pre numbering and decimation."

echo $RUN_EXPLANATION

####################################
source code-ANALYSIS.h.sh          #
source code-DOIT.h.sh              #
####################################
