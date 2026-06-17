echo SOURCED: code-HELPERS.h.sh
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

function uptimenow(){
    cat /proc/uptime | sed -n 's/\([0-9]*\.[0-9]*\) .*/\1/p'
    
}

function numdif(){
    echo $(dc -e "$1 $2 - p")
}

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

function uptimenow(){
    cat /proc/uptime | sed -n 's/\([0-9]*\.[0-9]*\) .*/\1/p'
    
}

function numdif(){
    echo $(dc -e "$1 $2 - p")
}


# numquotintzn M N echos rounddown(M/N) if this non-zero, 1 if zero.
# for pixel width of circles.
function numquotintnz(){
    echo $(dc -e "1 Sa $1 $2 / d 0 =a p")
}
# clumsy reverse polish calculator.
# Reg-a:=1 so we can put on top of stack if $1/$2 = 0.
# Push nums, /, if top == 0, push Reg-a's 1, else keep the quot.
# pop-print the 1 or the quot.
## used to calc the pixel width for a small circle.
### we store in reg-a (Sa) a 1 to return if the / is 0
### and duplicate the quotient to return if not!
### =a puts reg-a's 1 in the stack if quot==0.  UGH.

function mydate() {
    echo $(date +%b%d-%H-%M%S ) # May22-13-1435 MonthDay-Hr-MinSec
                                #Shell doesn't like colons
}


function echofold() {
    echo $@ | fold --spaces --width=100
}

function vev() {
    echo $1=${!1}
}

function vevs() {
    for x in ${1}
    do vev $x
    done
}
