
#!/bin/bash

# PRE-REQ: FFMPEG (any v) +ImageMagick (Win: command prompt not enough)
# General directions (ReadMe!):
# Change path below to proper path for your machine (Unix style though)
# Make sure you have a minimum of 780GB free on your HD (internal!)
# If impossible: you can still use the code but it will be much slower
# Choose the camera you want to run (A1,A2,A3,A4,B1,B2,B3,B4) in file=
# Follow that up by altering the tops of FLIRanalysisPhase1aCamX.cpp
# and FLIRanalysisPhase1bCamX.cpp to match the SAME CAMERA NAME (KEY!)
# After editing those source code files, you must save them, although
# since this script compiles them you won't need to manually recompile
# source C-TAP.sh to run (OR exec or . instead of source, OS-dependent)

# UFODAP: path, type avi, file, w=1, 360->800, don't make new vid and mv
# instructions for Windows users, if you have errors, go to the bottom:

path="."
type="mp4"  # case-sensitive
file="$path/N884A6_ch1_main_202409*.$type" #make it all of 1 cam eg ..$path/*_A1.
o=0
for m in $file
 do
    
    Prefix=`basename ${m%.$type}`
    echo $Prefix
    p=`ls -alth $file | wc -l`
    ((++o))
    echo "$o of $p movie file(s)"
    
    echo "Making Mick West redundant, processing ONI FOIA files..."
    FileName="${Prefix}.${type}"
    #rsync -Ph ${path}/${FileName} .
    mkdir -p bitmaps
    
    echo "Repaying TTSA investors, straightening Uri Gellar's spoons..."
    cd bitmaps/
    ffmpeg -threads 0 -hide_banner -an -i ../${FileName} -vf "scale=trunc(iw/4)*2:trunc(ih/4)*2,decimate,setpts=N/100/TB" -fps_mode vfr thumb%06d.bmp
    f=`ls -alth | wc -l`
    f=$((f-3))
    echo "$f frames captured"
    
    cd ../
    rm -f ImageProc.exe
    g++ -O3 -Wno-unused-result FLIRanalysisPhase1aCamX.cpp -o ImageProc.exe
    Input="${Prefix}.int"
    rm -f $Input
    
    u=0; v=0
    w=30
    t=$((f/w))
    echo "Removing any quantum woo; reticulating $t splines..."
    while [ $u -lt $f ]
    do
	./ImageProc.exe $u $t 0 >> $Input 2>> ${Prefix}.err
	((++v))
	echo Phase 1A: Part $v of $w Done
	if [ $v -gt $w ]
	then
	    echo "Don't panic -- there was a remainder from the division!"
	fi
	u=$((u+t))
	if [ $((u+t)) -gt $f ]
	then
	    t=$((f-u))
	fi
    done
    
    g++ -O3 FLIRanalysisPhase1bCamX.cpp -o ImageProc.exe
    Output="${Prefix}.out"
    rm -f $Output
    ff=`more ${Input} | wc -l`
    echo "$ff frames read"
    ./ImageProc.exe $Input $ff 0.5 > $Output
    echo "Phase 1B: Done!"
    n=`more ${Output} | wc -l` && echo "$n frames have objects?"
    if [ $n -gt 50000 ] || [ $n -eq 0 ]
    then
	if [ $n == 0 ]
	then
	    g++ -O3 -Wno-unused-result FLIRanalysisPhase1aCamX.cpp -o ImageProc.exe
	    u=0; v=0
            t=$((f/w))
            rm $Input
	    echo "Part deux: electric boogaloo..."
            while [ $u -lt $f ]
            do
		./ImageProc.exe $u $t 1 >> $Input 2> /dev/null
		((++v))
		echo Phase 1A: Part $v of $w Done
		if [ $v -gt $w ]
		then
                    echo "Don't panic -- there was a remainder from the division!"
		fi
		u=$((u+t))
		if [ $((u+t)) -gt $f ]
		then
                    t=$((f-u))
		fi
            done
	    g++ -O3 FLIRanalysisPhase1bCamX.cpp -o ImageProc.exe
	fi
	rm $Output
	./ImageProc.exe $Input $ff 0.98 > $Output
	echo "Phase 1B: Re-Done!" && n=`more ${Output} | wc -l`
    fi
    echo "$n frames have objects in them"

    # don't waste time on empty file OR overloading your HD
    if [ $n -gt 465000 ] || [ $n -eq 0 ]
    then
	rm -f ImageProc.exe; echo "Either too many or too few objects..."; continue
    fi
    # number above assumes 2.9 MB/image: (2.9*(46500+216e3))/1e3 = 760 GB of free space needed! Adjust for your machine
    
    echo "Logging range, deploying Little Green Men.."
    cat ${Output} | while read evt frame extr x y prob
    do
	((++frame))
	if (( $frame %5 == 0 ))
	then
	    echo "processing frame number" $frame
	fi
	i=$((x-9))
	j=$((y-9))
	k=$((x+9))
	l=$((y+9))
	if [ $frame -lt 10 ]
        then
            pad="00000"
        elif [ $frame -lt 100 ]
        then
            pad="0000"
        elif [ $frame -lt 1000 ]
	then
            pad="000"
        elif [ $frame -lt 10000 ]
        then
            pad="00"
        elif [ $frame -lt 100000 ]
        then
            pad="0"
        else
            pad=""
        fi
	if [ $y -lt 800 ]
	then
	    string="convert bitmaps/thumb$pad$frame.bmp -fill none -stroke cyan -draw 'circle $i,$j $k,$l' bitmaps/pic$pad$frame.bmp"
	else
	    string="convert bitmaps/thumb$pad$frame.bmp -fill none -stroke lime -draw 'circle $i,$j $k,$l' bitmaps/pic$pad$frame.bmp"
	fi
	eval $string
    done
    
    echo "Breaking Lue's NDA..."
    cd bitmaps/
    ext="MOV" #NOT same as type above!
    #mogrify -resize 640x360 "*" *.bmp #mogrify -format jpg *.bmp
    ffmpeg -threads 0 -r 60 -f image2 -pattern_type glob -i 'pic*.bmp' -vcodec libx264 -crf 25 -pix_fmt yuv420p ${Prefix}.${ext}
    #ffmpeg -i ${Prefix}.${ext} -filter:v "transpose=1,transpose=1" flipped.mp4
    
    echo "Cleaning detritus..."
    cd ../
    mv bitmaps/${Prefix}.${ext} .
    rm -rf ImageProc.exe *.cpp~ *.sh~ *.txt~ *.err bitmaps/
    echo "Cosmic consciousness has been achieved!!"
    
done

# buy a Mac or install Linux. Just kidding (But get Ubuntu for Windows!)
# remove all instances of -Ofast (or replace w/ -O3). There should be 4
# Change w=30 to higher nums (go into hundreds or even 1000 if need be)
# to avoid 'Killed' messages. Just make sure still divisible by 216,000
# If FFMPEG is slow i.e. speed=2x instead of ~7-8x from Mac
# change "-threads 0" by 1 until max possible of 16 (a multi-core comp)
# Zero is the default-but doesn't seem to optimize properly on Windows
# Most ideally you run this software on a machine which possesses GPUs!
# superuser.com/questions/1600352/hardware-acceleration-ffmpeg-on-windows
