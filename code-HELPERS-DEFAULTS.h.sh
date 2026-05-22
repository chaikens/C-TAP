echo SOURCED: code-HELPERS-DEFAULTS.h.sh
set -u #NOW, expanding undefined shell variables will give an error like in decent languages!
source code-HELPERS.h.sh

JOBNAME=$(basename $0 | sed -n 's/C-TAP-\(..*\).sh/\1/p')
if [ $JOBNAME"" = "" ]
then
    echo Your top level script should be named C-TAP-<jobname>.sh
    echo not $0
    echo Sorry you can recode all this though since it is open/free.
    exit 1
fi

#
# defaults
#

KEEP_ANY_OLD_XTERMS="" #user preference, also in case programs named xterm are used 
RUN_EXPLANATION=""
DEBUG=false
SOFTWARE_DIR=$(pwd)
FAST_FILESYS_DIR_IF_USED=
RESULTS_DIR="${SOFTWARE_DIR}/RESULTS-${JOBNAME}-$(mydate)"

BABY_MOVIE_CIRCLE_RAD_PIX=9

#Other and future uses will not use this, it's referred in the Phase1a command.
PHASE1A_OTHER_OPTIONS=""



#
# for ARCHITECTURE=framefile only
#
REUSE_BMPS=no
BITMAPS_PARENT_DIR=$(pwd)
BITMAPS_DIR_NAME="bitmaps"
SLOW_MOVIE_DIR=$(pwd)

#for ARCHITECTURE=pipeline only
PIPE_DIR=${SOFTWARE_DIR}

#Good only for framefile for now; for pipeline
#is set to "no" in code-SYSCONFIG.h.sh
MAKE_BABY_MOVIE="yes"


