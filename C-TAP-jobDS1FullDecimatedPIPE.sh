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
#redo Phase1b and babymovie making by setting this:
#
#USE_OLD_PHASE_1a_RESULT_N=2

#You could use this feature to experiment with the CamSett
#parameters, conveniently set in this script below, that
#do not change Phase1a results.

#When input movies are in a slow filesystem, you can specify
#where in big fast filesystem we will copy them for temporary use.
#(If the filesystems are the same, we don't copy.)
#
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
# are typically on secondary or removable drives. #
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

#Now for settings relevent for movies with common origin and details.
#All the settings are copied into the job run's log file.

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

# Information about FFMPEG_EXTRACT_FILTER
# Basically, we usually preprocess the input movies, using the scripts in the PP dir
# which are (currently) independent of this script.  When prepropressing is done,
# the filter set below should be empty.  This script uses ffmpeg only to extract frames.
#
#FFMPEG_EXTRACT_FILTER=""
#
# When you do not use preprocessing, the above filter string should do frame number drawing
# followed by any processing which would be part of the FLIR algorithm.  
#
# AND, this script should input the PREPROCESSED movies.
# 
# Explanation
# (1) We usually want the original frame numbers to be drawn into each frame.  That
#     will be done by the first filter of the ffmpeg run on the original movie.
# (2) Often but not always we want the first ffmpeg run to decimate, change encoding,
#     or reduce resolution. The FLIR Algorithm sometimes requires the frame sequence
#     to be decimated, when the objects move slowly. So, frame times get lost.
# (3) The first ffmpeg run result movis is used twice: First it provides the input
#     frames to Phase1a, and second the frames selected by Phase1b to make the baby movie.
# (4) Convenience and practicality on very short test/demonstration movies lead us to
#     skip preprocessing.  So then, we encode the frame numbering and decimation, etc.,
#     in the filter we use:
#
# Full (Resolution) setting below differs from Half only in that full omits the filter
# scale=trunc(iw/4)*2:trunc(ih/4)*2
FFMPEG_EXTRACT_FILTER='-filter:v drawtext=fontfile=arial.ttf:text=%{n}:x=(w-tw)-140:y=(2*lh)-80:fontcolor=white:box=1:boxcolor=0x00000099:fontsize=60,decimate=cycle=5,setpts=N/100/TB -vsync vfr'

#Beware: ffmpeg option names are -single_dash_underscore_sep.
#Whilst our C++ commands use Unix style --double-dash-dash-sep.

# Eventually we may want to use -filter_complex
# setpts is set presentation timestamps

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

#on the other hand, for HALF FRAME, (DECIMATED) extraction.
#    MOVIE_TO_FRAME_DIV=2 #used by the pipe architecture, but not yet here.
#    MOVIE_SCALE_OPTION="--movie-scale 1"
#    PIXPROC_SCALE_OPTION="--pixproc-scale 1"
#    USER_SCALE_OPTION="--user-scale 1"
#    OTHER_OPTIONS="--camera-index 1"

##### Explain your run into the log at top and for each movie###

RUN_EXPLANATION="Explanation: ffmpeg .mp4----> decimated frame (ffmpeg filter) .yuv raw (our table based YUVtoBMP pipeline)----->.bmp sequence -----> (Phase1aPipe) (redirects to/from files) > movie.int > (Phase1bPipe) > file.out  This is coded by $0"

echo $RUN_EXPLANATION

####################################
source code-ANALYSIS.h.sh          #
source code-DOIT.h.sh              #
####################################
