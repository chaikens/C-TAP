set -u #NOW, expanding undefined shell variables will give an error like in decent languages!

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
