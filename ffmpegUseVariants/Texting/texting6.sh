ffmpeg -i DS.mp4 -vf "drawtext=fontfile=arial.ttf: text=%{n}: x=(w-
tw)-140: y=(2*lh)-60: fontcolor=white: box=1: boxcolor=0x00000099:
fontsize=60" -t 20 -y out6.mp4

ffplay out6.mp4


