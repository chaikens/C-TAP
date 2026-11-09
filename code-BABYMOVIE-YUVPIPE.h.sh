


pipe_yuv_to_filter="${PIPE_DIR}/${JOBNAME}PIPE.yuv" #should end in .yuv to tell ffmpeg format
#This pipe filename expression is hardcoded in our shell fn ffmpeg_pipe_extract,
#which also was used for Phase1a extraction:  Same ffmpeg filter and other parameters.
#WARNING: THIS MUST EQUAL "${PIPE_DIR}/${JOBNAME}PIPE.yuv" used in code-ANALYSIS.h.sh: ffmpeg_pipe_extract()


rm -f ${pipe_yuv_to_filter} 
mknod ${pipe_yuv_to_filter} p

pipe_yuv_from_filter="${PIPE_DIR}/${JOBNAME}PIPE2.yuv" #should end in .yuv to tell ffmpeg format
rm -f ${pipe_yuv_from_filter}  
mknod ${pipe_yuv_from_filter} p

echo STEP: "Starting baby making yuv extract-->BabyFilter--->MOV pipeline" Please wait...

if [ ${xterm_ffmpeg_pid}x = "x" ]
then
    touch ${RESULTS_DIR}/ffmpeg.log
    xterm -bg '#E0FFFF' ${XTERM_PARAM} -geometry 160x30+0+100 -title 'ffmpeg extract bitmaps'  -e tail -f ${RESULTS_DIR}/ffmpeg.log &
    #xterm -geometry 160x30+0+100 -title 'ffmpeg extract bitmaps'  -e less -f +F ${RESULTS_DIR}/ffmpeg.log &
    xterm_ffmpeg_pid=$!  #to ensure just one xterm for ffmpeg; may kill at end.
fi

sttime=$(uptimenow)
echo "TIME: Baby pipeline ffmpeg->yuv->BabyFilter->ffmpeg->MOV began " $sttime "sec." >> $LOG

inwidth=$(widthOfMovie $movie_file)   #input
inheight=$(heightOfMovie $movie_file) #input

if (($MOVIE_SCALE > $PIXPROC_SCALE))
then
    width=$((inwidth * PIXPROC_SCALE / MOVIE_SCALE))
    height=$((inheight * PIXPROC_SCALE / MOVIE_SCALE))
else
    width=$inwidth
    height=$inheight
fi

imgdim="${width}x${height}"
#trying to do this first, before starting extraction.

# 3 Stage Pipeline


# Stage 1
$(ffmpeg_pipe_extract ${movie_file}) &
#ffmpeg_pipe_extract shell function already has $time_cmd_prefix and ffmpeg.log logging.


#Stage 2
#BabyFilter hard codes fd=3 for yuv input, fd=4 for yuv output
${time_cmd_prefix} ${SOFTWARE_DIR}/BabyFilter --phase-1b-file ${phase1b_out} --offset 1 ${imgdim} 3< ${pipe_yuv_to_filter} 4> ${pipe_yuv_from_filter} 2>>${LOG} &

#Not put in jobname.cmds.n because the 3 file redirections didn't occur when the script assigned the above line, "quoted", to a variable,
#(that variable echo'ed to the cmds log), and that variable run as $var &
#We don't really need the cmds log when we time the commands, because the verbosed time command puts the cmd line in the log.
#I guess the redirections should be done to the time command's process, not the timed command.
#More systems research might give a better fix.


#echo BEFORE STARTING STAGE 3 OF YUVPIPE BABYMAKER, A TIMED CALL TO FFMPEG
#stty -g
#Stage 3
${time_cmd_prefix} ffmpeg -hide_banner -y -s $imgdim -r 60 -i $pipe_yuv_from_filter  ${RESULTS_DIR}/${generic_baby_movie_name} &>>${RESULTS_DIR}/ffmpeg.log

#eval ffmpeg -hide_banner -y -s $imgdim -r 60 -i $pipe_yuv_from_filter  -crf 25 "${RESULTS_DIR}/${generic_baby_movie_name}" &>>${RESULTS_DIR}/ffmpeg.log

#from the original baby making code,
#$FFMPEG -s $S -threads 0 -r 60  -i $SrcToBabyPipe  -vcodec libx264 -crf 25 -pix_fmt yuv420p $BABYMOV
#                                   dont need.......|-------------|
#$FFMPEG -s $S -threads 0 -r 60  -i $SrcToBabyPipe   -crf 25 -pix_fmt yuv420p $BABYMOV
#                                   dont need................|--------------|

#echo AFTER STAGE 3 WE HAVE WEIRDNESS
#stty -g

#
# Here's a story of weirdness.
# In ALL cases, when the -crf 25 option is in the above ffmpeg baby movie making cmd, the
# terminal from which we run C-TAP-jobname.sh gets it settings changed, and they dont
# change when -crf 25 is omitted and all things being equal!!!


# The /usr/bin/time command on the ffmpeg command and arguments
# coded above worked EXCEPT it left the invoking shell's stty settings an abnormal state, including
# that there was no echoing and the CR key was not interpreted as an end of shell line.
# So, I removed the command timing, and invoked ffmpeg via the bash eval builtin.
#(ffmpeg logging only goes to cwd and has no append to log file option,
# so I can't get dynamic monitoring that way).  We'll just have to do without timing.
#If you want timing, replace eval with ${time_cmd_prefix} and fix your messed up shell terminal
# reset when the job is done.  (This all worked fine when runs were scripted.)

#IT WORKS FINE WHEN THE -crf 25 option to ffmpeg IS REMOVED!  -crf is an option to stty!  UGGHH

#Here is the original state interleaved with the abnormal state, reported by stty -g:
#speed 38400 baud; rows 36; columns 120; line = 0;
#
#intr = ^C; quit = ^\; erase = ^?; kill = ^U; eof = ^D; eol = <undef>; eol2 = <undef>; swtch = <undef>; start = ^Q;
#intr = ^C; quit = ^\; erase = ^?; kill = ^U; eof = ^D; eol = <undef>; eol2 = <undef>; swtch = <undef>; start = ^Q;
#
#stop = ^S; susp = ^Z; rprnt = ^R; werase = ^W; lnext = ^V; discard = ^O; min = 1; time = 0;
#stop = ^S; susp = ^Z; rprnt = ^R; werase = ^W; lnext = ^V; discard = ^O; min = 1; time = 0;
#
#-parenb -parodd -cmspar cs8 -hupcl -cstopb cread -clocal -crtscts
#-parenb -parodd -cmspar cs8 -hupcl -cstopb cread -clocal -crtscts
#
#-ignbrk -brkint -ignpar -parmrk -inpck -istrip -inlcr -igncr  icrnl  ixon -ixoff -iuclc -ixany -imaxbel iutf8
#-ignbrk -brkint -ignpar -parmrk -inpck -istrip -inlcr -igncr -icrnl -ixon -ixoff -iuclc -ixany -imaxbel iutf8
#--------------------------------------------------------------^------^
#
#
#opost -olcuc -ocrnl onlcr -onocr -onlret -ofill -ofdel nl0 cr0 tab0 bs0 vt0 ff0
#opost -olcuc -ocrnl onlcr -onocr -onlret -ofill -ofdel nl0 cr0 tab0 bs0 vt0 ff0
#
#isig  icanon  iexten  echo echoe echok -echonl -noflsh -xcase -tostop -echoprt echoctl echoke -flusho -extproc
#isig -icanon -iexten -echo echoe echok -echonl -noflsh -xcase -tostop -echoprt echoctl echoke -flusho -extproc
#-------^-------^-------^
#
#4500:5:bf:8a3b:3:1c:7f:15:4:0:1:0:11:13:1a:0:12:f:17:16:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0
#4000:5:bf:0a31:3:1c:7f:15:4:0:1:0:11:13:1a:0:12:f:17:16:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0:0




rm -f ${pipe_yuv_to_filter} ${pipe_yuv_from_filter}

echo "TIME: Baby pipeline runtime was " $(numdif $(uptimenow) $sttime) " sec." >> ${LOG}
echo "Baby making pipeline finished, " $(numdif $(uptimenow) $sttime) " sec." 

echo YUVPIPE ending
stty -g
