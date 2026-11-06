#!/usr/bin/bash
#FOR STANDARD USAGE, EDIT ONLY      #
#THE FILE-NAME OF THIS SCRIPT       #
#AND CODE OUTSIDE BOXES LIKE THIS.  #
#THE FILE-NAME SHOULD HAVE THE FORM #
#  C-TAP-<jobname>.sh               #
source code-HELPERS-DEFAULTS.h.sh   #
echo STEP: Do and check settings.   #
#####################################

#When you had a good run numbered N, you could just
#redo Phase1b and babymovie by setting this:
#USE_OLD_PHASE_1a_RESULT_N=

#When input movies are in a slow filesystem, you can specify
#where in big fast filesystem we will copy them for temporary use.
#(If the filesystems are the same, we don't copy.)
OPT_BIG_FAST_DIR="/media/seth/CTAP"

PHASE_1a=F1a.orig #keep for devel

ARCHITECTURE="pipeline"  #keep for devel
#"framefile" #vs #pipeline

###########END OF users' system configuration settings###
source code-SYSCONFIG.h.sh                              #
#########################################################

###################################################
# Please configure here which movie(s) to process #
# SLOW_MOVIE_DIR default is cwd of this script,   #
# but it is convenient to change it since movies  #
# are typically on removable media.               #
###################################################
#SLOW_MOVIE_DIR=$(pwd)

ext=mp4  #TO DO--refactor to a CAP_UND style USER_SETTING, not internal-var.
movie_files="$SLOW_MOVIE_DIR/DroneShort1FullDecimated.$ext"
#### note-we use different names for the same movie
#    to separate results of different experiments.

###################################################################
#Here is how to configure a dir full of movies for research, not  #
# testing: movie_files="$SLOW_MOVIE_DIR/N884A6_ch1_main_*.$ext"   #
###################################################################

#################################
source code-MOVIES.h.sh         #
#################################

#Now for settings relevent for movies with common origin and details

#Don't use default CamSett.txt file
#OPT_CamSett="--CamSett-file $(pwd)/CamSett.txt"
cat > tempCamSett.txt <<EOF
smallestThr= 0
biggestThr= 256
smallestPix= 0
biggestPix= 300
SkewGaussAmpl= 0.633
SkewGaussXi= 2.2
SkewGaussOmega= 2.1
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


#The FLIR Algorithm requires the frame sequence to be decimated.
#So, frame times get lost when extracted!

#ffmpeg option notes.  We use
## option -vf (alias -filter:v) <video filter script> 
# Eventually we may want to use -filter_complex

#Beware: ffmpeg option names are -single_dash_underscore_sep.
#Whilst our C++ commands use Unix style --double-dash-dash-sep.

#For FULL FRAME with DECIMATION extraction by ffmpeg
#Also, the 2nd filter setpts is set presentation timestamps
FFMPEG_EXTRACT_FILTER="-vf decimate,setpts=N/100/TB"

#for HALF-RESOLUTION, ditto (simply add the scale filter)
#FFMPEG_EXTRACT_FILTER="-vf scale=trunc(iw/4)*2:trunc(ih/4)*2,decimate,setpts=N/100/TB" 

#for HALF-RESOLUTION with DECIMATION extraction.
#Also, the 2nd filter setpts is set presentation timestamps
#FFMPEG_EXTRACT_FILTER="-vf scale=trunc(iw/4)*2:trunc(ih/4)*2,decimate,setpts=N/100/TB" 

# THIS DroneShort1.mov ONLY: In the Phase1a C++ program, we wrote an exclusion
# zone function for DroneShort1, and also named a camera for it.
#for the DroneShort1.mov only, both pipeline and framebmp arch,
#(1) --camera-index 1 selects the exclusion zone to hide the data/time display.
#    We hard coded this and camera name DroneShort1.
#(2) CROP half-way down, 1080/2 to eliminate the trees. (more than original Camsett)

# These are used for both pipeline and frame .bmp architectures.



#for FULL FRAME resolution, DECIMATED/pts extraction: 

MOVIE_TO_FRAME_DIV=1 #used by the pipe architecture, but not yet here.

MOVIE_SCALE=2     #the original or preprocessed movie		      
PIXPROC_SCALE=2	  #FLIR processing and baby movie		      
USER_SCALE=1	  #Position, size, crops in CamSett and exclusion zones

OTHER_OPTIONS=" --camera-index 1"

PHASE1A_OTHER_OPTIONS="--crop-args --CROP_XI 0  --CROP_XF 540 --CROP_YI 0 --CROP_YF 1920"

#PHASE1A_OTHER_OPTIONS="--crop-args --CROP_XI 0  --CROP_XF 540 --CROP_YI 960 --CROP_YF 1920"

#for HALF FRAME, (DECIMATED) extraction.
# COMMENTED OUT
#    MOVIE_TO_FRAME_DIV=2 #used by the pipe architecture, but not yet here.
#    MOVIE_SCALE_OPTION="--movie-scale 1"
#    PIXPROC_SCALE_OPTION="--pixproc-scale 1"
#    USER_SCALE_OPTION="--user-scale 1"
#    OTHER_OPTIONS="--camera-index 1"
#    PHASE1A_OTHER_OPTIONS="--crop-args --CROP_XI 0  --CROP_XF 540 --CROP_YI 0 --CROP_YF 1920"
##### Explain your run into the log at top and for each movie###

RUN_EXPLANATION="ffmpeg .mp4----> decimated frame (ffmpeg filter) .yuv raw (our table based YUVtoBMP pipeline)----->.bmp sequence -----> (Phase1aPipe) (redirects to/from files) > movie.int > (Phase1bPipe) > file.out  This is coded by $0"

echo $RUN_EXPLANATION

####################################
source code-ANALYSIS.h.sh          #
source code-DOIT.h.sh              #
####################################
