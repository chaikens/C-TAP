NAME=newNrSaDcMixedSetptsVfr

FF=/media/seth/TerabyteSandisk/ffmpegprefix/bin/ff

/usr/bin/time -o $NAME.time -v ${FF}mpeg  -i DS.mp4 -an -filter:v 'drawtext=fontfile=arial.ttf:text=%{n}:x=(w-tw)-140:y=(2*lh)-100:fontcolor=white:box=1:boxcolor=0x00000099:fontsize=60,scale=trunc(iw/4)*2:trunc(ih/4)*2,decimate=cycle=5:mixed=true,setpts=N/100/TB' -fps_mode vfr -y $NAME.mp4 &>> $NAME.fflog


/usr/bin/time -o $NAME.time -v ${FF}mpeg  -i $NAME.mp4 -an -filter:v 'drawtext=fontfile=arial.ttf:text=%{n}:x=(w-tw)-200:y=(2*lh)-50:fontcolor=white:box=1:boxcolor=0x00000099:fontsize=30' -y $NAME.numb.mp4 &>> $NAME.fflog

${FF}play $NAME.mp4 &
${FF}play $NAME.numb.mp4 &







