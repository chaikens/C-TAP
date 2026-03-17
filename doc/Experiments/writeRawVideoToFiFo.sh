#make the fifo with
#mknod DS.yuv p
#We must start reader before we start writer
ffmpeg -hide_banner -y -i DroneShort1.MOV -t 5 DS.yuv
# -y -> Overwrite output files without asking. 
