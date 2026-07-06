ffmpeg -i DS.mp4 -vf "drawtext=fontfile=arial.ttf: text=%{n}: x=(w-
tw)/2: y=h-(2*lh): fontcolor=white: box=1: boxcolor=0x00000099:
fontsize=72" -y out.mp4

ffplay out.mp4


