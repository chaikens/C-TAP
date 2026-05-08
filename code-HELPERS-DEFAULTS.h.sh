echo SOURCED: code-HELPERS-DEFAULTS.h.sh
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

REUSE_BMPS=no
FAST_FILESYS_DIR_IF_USED=
DEBUG=false
BITMAPS_PARENT_DIR=$(pwd)
BITMAPS_DIR_NAME="bitmaps"
SLOW_MOVIE_DIR=$(pwd)

SOFTWARE_DIR=$(pwd)
RESULTS_DIR="${SOFTWARE_DIR}/RESULTS-${JOBNAME}"
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

function exit_greeting() {
echo "True, full consciousness can now be reasserted by you poor human user."

echo "See the results in the xterms"
echo -n "Type yes to kill the xterms. Or, no, or wait 20sec:"
if ! read -t 20 yes
then
    echo OK. Time for asking done, leaving the xterms.
    echo Kill them manually or use ./killxterms.sh
fi

if [ ${yes}"" = "yes" ]
then
    kill ${Phase1b_xterm_PID}
    kill ${Phase1a_xterm_PID}
    kill ${Phase1b_mess_xterm_PID}
    kill ${Extract_xterm_PID}
fi

echo $0 Done.
exit 0
}

