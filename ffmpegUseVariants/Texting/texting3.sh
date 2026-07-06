/media/seth/TerabyteSandisk/ffmpegprefix/bin/ffmpeg -i out.mp4 -vf 'scale=trunc(iw/4)*2:trunc(ih/4)*2,decimate,setpts=N/100/TB' -fps_mode vfr -y out3.mp4

ffplay out3.mp4


