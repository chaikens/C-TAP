echo SOURCED: code-HELPERS-DEFAULTS.h.sh
set -u #NOW, expanding undefined shell variables will give an error like in decent languages!
JOBNAME=$(basename $0 | sed -n 's/C-TAP-\(..*\).sh/\1/p')
if [ $JOBNAME"" = "" ]
then
    echo Your top level script should be named C-TAP-<jobname>.sh
    echo not $0
    echo Sorry you can recode all this though since it is open/free.
    exit 1
fi

read -t 18 -p "Keep running any xterm windows?(no, or yes is default)" 
if [ $REPLY"x" = "nox" ]
then
    killall xterm
fi


# defaults
#

RUN_EXPLANATION=""
DEBUG=false
SOFTWARE_DIR=$(pwd)
FAST_FILESYS_DIR_IF_USED=
RESULTS_DIR="${SOFTWARE_DIR}/RESULTS-${JOBNAME}"

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


#For now, we change (mv) its name to save it.

#
#  Helper shell functions
#

#Phase1a reads depth 24 (8x8x8) .bmps only
function depthOfBmpIs24()
{
    if [ ! -r $1 ]
    then
	return 1
    else
        d=$( file $1 | sed 's/.*format, \([0-9]\+\) x \([0-9]\+\) x \([0-9]\+\).*/\3/' )
        #[sdc trial/err->] substitute/match..widthdigs x heightdigs  x depthdigs  .*/ print depth/
  	#thumb000001.bmp: PC bitmap, Windows 3.x format, 1920 x 1080 x 24, image size 6220800, cbSize 6220854, bits offset 54
	if [ $d"" = 24 ]
	then
            return 0
	else
	    return 1
        fi
    fi	
}
function widthOfBmp()
{
    if [ ! -r $1 ]
    then
	return 1
    else
        echo $( file $1 | sed 's/.*format, \([0-9]\+\) x \([0-9]\+\) x \([0-9]\+\).*/\1/' )
        #[sdc trial/err->] substitute/match..widthdigs x heightdigs  x depthdigs  .*/ print width/
  	#thumb000001.bmp: PC bitmap, Windows 3.x format, 1920 x 1080 x 24, image size 6220800, cbSize 6220854, bits offset 54
	return 0
    fi
}
function heightOfBmp()
{
    if [ ! -r $1 ]
    then
	return 1
    else
       echo $( file $1 | sed 's/.*format, \([0-9]\+\) x \([0-9]\+\) x \([0-9]\+\).*/\2/' )
    	return 0
    fi
}	 
function widthOfMovie() {
    ffprobe -v quiet  -show_streams $1 | sed -n 's/^width=\([1-9][0-9]*\)$/\1/p'
}

function heightOfMovie() {
    ffprobe -v quiet  -show_streams $1 | sed -n 's/^height=\([1-9][0-9]*\)$/\1/p'
    }

function padTo6Digs() {
    if [ $1 -lt 10 ]
    then
        pad="00000"
    elif [ $1 -lt 100 ]
    then
        pad="0000"
    elif [ $1 -lt 1000 ]
    then
        pad="000"
    elif [ $1 -lt 10000 ]
    then
        pad="00"
    elif [ $1 -lt 100000 ]
    then
        pad="0"
    else
	if [ !$? ]
	then
	    return 1
	fi
        pad=""
    fi
    echo "${pad}$1"
}

function exit_greeting() {
echo "True, full consciousness can now be reasserted by you poor human user."

echo "See the results in the xterms and ${RESULTS_DIR}"
echo -n "Type yes to kill the xterms. Or, no, or wait 20sec:"
if ! read -t 20 yes
then
    echo OK. Time for asking done, leaving the xterms.
    echo Kill them manually or use ./killxterms.sh
fi

if [ ${yes}"" = "yes" ]
then
    if [ ${xterm_pids-none} != "none" ]
    then
	kill ${xterm_pids[*]} 
    fi
fi

echo $0 Done.
exit 0
}

function uptimenow(){
    cat /proc/uptime | sed -n 's/\([0-9]*\.[0-9]*\) .*/\1/p'
    
}

function numdif(){
    echo $(dc -e "$1 $2 - p")
}

