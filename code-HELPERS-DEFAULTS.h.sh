echo SOURCED: code-HELPERS-DEFAULTS.h.sh
set -u #NOW, expanding undefined shell variables will give an error like in decent languages!
source code-HELPERS.h.sh
start_dir=$(pwd)

JOBNAME=$(basename $0 | sed -n 's/C-TAP-\(..*\).sh/\1/p')
if [ $JOBNAME"" = "" ]
then
    echo 'Your top level script should be named C-TAP-<jobname>.sh'
    echo not $0
    echo Sorry you can recode all this though since it is open/free.
    exit 1
fi

#DOC: width height
#We try to set these when needed automatically now.
#After testing, please remove any settings in C-TAP-job.sh scripts.
#When Phase1a is run, the input movie file is probed by ffmpeg to set these
#However, when PHASE_1a_OLD_RESULT_N=nnn is set so movie.out.nnn
#is reused, the movie is not probed (since we don't need it and may have
#removed or reused its storage).  Then, width and height should be set
#manually.


#
# defaults
#

#DOC: NEW_BABY_DEVEL
# for now, new means the baby movie frames are selected by a filter
# on the output of a 2nd run of ffmpeg that writes yuv frames to a pipe.
# When set to no, bitmaps must have been extracted and they are used
# to make the baby movie
NEW_BABY_DEVEL=yes

#DOC: TRY_CONVERT_picbmp_TO_picbmpsee code-BABYMOVIE-BMP.h.sh
# variations for performance improving development
TRY_CONVERT_picbmp_TO_picbmp="no"  # see code-BABYMOVIE-BMP.h.sh
ALLOW_THRASHING_BABIES="no"        # yes will disable semaphore mutexing of baby making.
#IDEA!! Use >1 initialize semaphore to limit HOW MANY baby makers compete!

xterm_ffmpeg_pid=  #not for config.  empty, store single pid so we can test and eventually kill

#DOC: KILLALL_XTERMS_AT_START control whether to run killall xterm near the beginning
KILLALL_XTERMS_AT_START="no" #default is do killall xterm near beginning
#so xterms still running after we fail will be killed.
#Yes may be user preference, also in case they have  named xterm are used
#no is a good choice when running concurrent jobs!

#DOC: KILL_OUR_XTERMS_BEFORE_EACH_MOVIE
KILL_OUR_XTERMS_BEFORE_EACH_MOVIE="yes"
#no is a good choice when analyzing single movies

#DOC: KILL_XTERMS_DONT_ASK
KILL_XTERMS_DONT_ASK="no"
#default: Ask user at shell to kill our xterms, give 20 sec to answer yes.

#DOC: XTERM_PARAM=
XTERM_PARAM=" -fn 7x13bold "

#DOC: RUN_EXPLANATION will be printed in the .log file
RUN_EXPLANATION=""

#DOC: DEBUG currently not used
DEBUG=false

#DOC: SOFTWARE_DIR where C-TAP .sh scripts and unix executables live.
SOFTWARE_DIR=$(pwd)


#DOC: OPT_BIG_FAST_DIR directory for subdirs we make for
#bitmaps (eventually perhaps others).  The filesys should be large.
#After possibly overridden in C-TAP-job.sh,  it's only used to help define
# BITMAPS_PARENT_DIR
# BITMAPS_DIR_NAME
# BITMAPS_DIR
OPT_BIG_FAST_DIR=$(pwd)

#DOC: RESULTS_DIR where result files for job will be stored.
RESULTS_DIR="${SOFTWARE_DIR}/RESULTS-${JOBNAME}"

#DOC: USE_OLD_PHASE_1a_RESULT_N control whether or not to run Phase1a
#What to do
#Keep "" to do it in this run.
#Set to an old run number to skip and use the result of run N
# for the next phase
USE_OLD_PHASE_1a_RESULT_N=

#DOD: PHASE_1a executable name in SOFTWARE_DIR.
PHASE_1a="F1a.orig"

#DOC: PHASE_1b_RESULT_N_OR_NONE
#intention is to skip 1b processing, say to modify features of the baby movie.
PHASE_1b_RESULT_N_OR_NONE=

#DOC: PHASE_1b_VERBOSE gives the Phase1b option
# " --verbose-file $phase1b_verbose_base "
# phase1b_verbose_base is local to code_ONEMOVIE.h.sh
PHASE_1b_VERBOSE=no

#DOC: MAKE_BABY_MOVIE name tells all
MAKE_BABY_MOVIE=yes


#DOC: EXT baby movie extension
EXT="MOV" #baby movie extension

#DOC: KEEP_BABY_BMPS
KEEP_BABY_BMPS=no

#DOC: PHASE1A_OTHER_OPTIONS
#Other and future uses will not use this, it's referred in the Phase1a command.
PHASE1A_OTHER_OPTIONS=""


#
# for ARCHITECTURE=framefile only
#
#DOC: REUSE_BMPS
REUSE_BMPS=no

#DOC: BITMAPS_PARENT_DIR
#for BABY MOVIE always, and also pipeline
BITMAPS_PARENT_DIR=$(pwd)

#DOC: BITMAPS_DIR_NAME
#for BABY MOVIE always, and also pipeline
BITMAPS_DIR_NAME="bitmaps"

#DOC: SLOW_MOVIE_DIR Directory for the input movies.
# Our default is that they should be preprocessed.
SLOW_MOVIE_DIR=$(pwd)

#DOC: FAST_MOVIE_DIR
#Set this so that when the next movie is to be done,
#it is copied to a faster dir, used from there, and
#the copy is deleted when done.
FAST_MOVIE_DIR=$(pwd)/TMPMOVIEDIR



#In no case do we delete the original copy.

#DOC: PIPE_DIR where named pipe (files) are created, if any.
#for ARCHITECTURE=pipeline only
PIPE_DIR=${SOFTWARE_DIR}

#DOC: MAKE_BABY_MOVIE 
#Good only for framefile for now; for pipeline
#is set to "no" in code-SYSCONFIG.h.sh
MAKE_BABY_MOVIE="yes"

#DOC: PHASE_1b_LEVEL
#1b Analysis parameter (NOT in CamSett!)
PHASE_1b_LEVEL=0.5

#DOC: PHASE_1b_LEVEL_REDO
#1b Analysis parameter (NOT in CamSett!)
PHASE_1b_LEVEL=0.5
PHASE_1b_LEVEL_REDO=0.98

#DOC: OPT_CamSett
#set in C-TAP-job.sh script to "" or "--CamSett-file <path to a CamSett file>"
#If OPT_CamSett="" the C++ programs will read the (default) $SOFTWARE_DIR/CamSett.txt
#
# We conveniently embed CamSett file contents into the C-TAP-job.sh script
# with a "here" document redirecting a cat command to a file (tempCamSett.txt)
# and setting OPT_CamSett="--CamSett-file $(pwd)/tempCamSett.txt" 
OPT_CamSett=

#
#  shell functions specific to us
#

#DOC: xterm_pids internal, lists pids of xterms we kill at the end of a movie run.
xterm_pids=
function kill_our_xterms() {
    #do it by PID
    if [ "${xterm_pids}none" != "none" ]
    then
	kill ${xterm_pids[*]}
	xterm_pids=
    fi
}


function exit_greeting() {
echo "True, full consciousness can now be reasserted by you poor human user."

#the exiting skit is clumsy on slow ssh connections, so we don't do it for now

echo "See the results in the xterms."
cd ${start_dir}
if [ ${KILL_XTERMS_DONT_ASK}"" = "yes" ]
then
    if [ "${xterm_pids}"none != "none" ]
    then
	$(kill_our_xterms) 
    fi
else
    echo -n "Type yes to kill the xterms. Or, no, or wait 30sec:"
    if ! read -t 30 yes
    then
	echo OK. Time for asking done, leaving the xterms.
	echo Kill them manually or use ./killxterms.sh
    else
	if [ ${yes}"x" = "yesx" ]
	then
	    kill_our_xterms
	    if [ ${xterm_ffmpeg_pid}x != "x" ]
	    then
	       kill ${xterm_ffmpeg_pid}
	    fi
	fi
    fi
fi
echo $0 Done
if [ $MAKE_BABY_MOVIE"wawa" = "yeswawa" ]
then
    if [ ${phase1b_failed}"wawa" = "nowawa" ]
    then
	echo "cd ${RESULTS_DIR} ; pwd; ffplay -hide_banner -f mov ${baby_movie_logn}" > m
	#echo I created file m which contains
	#cat m
	echo
	echo 'ffplay keys: --DownArrow-- to back up, --s-- to step frame, --space-- to play/pause' 
	echo 'To see the Baby Movie, type source m (or . <space> m), it puts you in RESULTS dir. too'
	echo
    else
	echo
	echo "cd ${RESULTS_DIR}; pwd " > m
	echo 'Phase1b failed (no events), no baby movie, sorry.'
	echo 'To cwd to RESULTS dir, type source m (or . <space> m)'
	echo
    fi
else
        echo
	echo No baby movie requested.
	echo "cd ${RESULTS_DIR}; pwd " > m
	echo To cwd to RESULTS dir ${RESULTS_DIR}, type source m '(or . <space> m)'
	echo
fi

#
# THIS IS WEIRD
# Somehow, we end up with -echo in terminal settings at this point.
#Why is a mystery.  There are commented out DEBUG operations that had
#been used to try to sort this out.  I'm giving up on this for now
#
stty echo
}






