ffmpeg -i out.mp4 -vf 'scale=trunc(iw/4)*2:trunc(ih/4)*2,decimate,setpts=N/100/TB' -y out2.mp4

ffplay out2.mp4


