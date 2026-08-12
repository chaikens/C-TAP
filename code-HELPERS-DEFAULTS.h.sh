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

#
# defaults
#

NEW_BABY_DEVEL=no


# variations for performance improving development
TRY_CONVERT_picbmp_TO_picbmp="no"  # see code-BABYMOVIE-BMP.h.sh
ALLOW_THRASHING_BABIES="no"        # yes will disable semaphore mutexing of baby making.
#IDEA!! Use >1 initialize semaphore to limit HOW MANY baby makers compete!

xterm_ffmpeg_pid=  #empty, store single pid so we can test and eventually kill

KILLALL_XTERMS_AT_START="no" #default is do killall xterm near beginning
#so xterms still running after we fail will be killed.
#Yes may be user preference, also in case they have  named xterm are used
#no is a good choice when running concurrent jobs!

KILL_OUR_XTERMS_BEFORE_EACH_MOVIE="yes"
#no is a good choice when analyzing single movies

KILL_XTERMS_DONT_ASK="no"
#default: Ask user at shell to kill our xterms, give 20 sec to answer yes.

XTERM_PARAM=" -fn 7x13bold "

RUN_EXPLANATION=""

DEBUG=false

SOFTWARE_DIR=$(pwd)

FAST_MOVIE_DIR=$(pwd)/TMPMOVIEDIR
FAST_FILESYS_DIR_IF_USED=
RESULTS_DIR="${SOFTWARE_DIR}/RESULTS-${JOBNAME}"

#What to do
#Keep "" to do it in this run.
#Set to an old run number to skip and use the result of run N
# for the next phase
#Set to NONE to skip that phase, to use old data from the next phase.
PHASE_1a_RESULT_OLD_N_OR_NONE=

PHASE_1a="F1a.orig"
PHASE_1b_RESULT_N_OR_NONE=

MAKE_BABY_MOVIE=yes
EXT="MOV" #baby movie extension
KEEP_BABY_MAKING_FILES=no
KEEP_BABY_BMPS=no

#Other and future uses will not use this, it's referred in the Phase1a command.
PHASE1A_OTHER_OPTIONS=""


#
# for ARCHITECTURE=framefile only
#
REUSE_BMPS=no

#for BABY MOVIE always, and also pipeline
BITMAPS_PARENT_DIR=$(pwd)
BITMAPS_DIR_NAME="bitmaps"

SLOW_MOVIE_DIR=$(pwd)

FAST_MOVIE_DIR=
#Set this so that when the next movie is to be done,
#it is copied to a faster dir, used from there, and
#the copy is deleted when done.

#In no case do we delete the original copy.

#for ARCHITECTURE=pipeline only
PIPE_DIR=${SOFTWARE_DIR}

#Good only for framefile for now; for pipeline
#is set to "no" in code-SYSCONFIG.h.sh
MAKE_BABY_MOVIE="yes"

#Analysis parameters
PHASE_1b_LEVEL=0.5
PHASE_1b_LEVEL_REDO=0.98

#
#  shell functions specific to us
#

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
exit 0
}






