/media/seth/TerabyteSandisk/ffmpegprefix/bin/ffmpeg -i DS.mp4 -vf 'scale=trunc(iw/4)*2:trunc(ih/4)*2,decimate,setpts=N/100/TB'  -y out5.mp4

ffplay out5.mp4


