
PATH := .:$(PATH)

all:  clean DQS.fmg.*.bmp  DQS.fmgPipe* UnivTest.bmp

.PRECIOUS : DroneCalib1.mp4 DroneShort1.mp4 DQS.mp4 *.messages
# I think (not sure) this prevents that make deletes *.messages
# when there's an error?  But our clean deletes them.

DroneCalib1.mp4 :
	echo "Contact C-TAP team."

DroneShort1.mp4 : DroneCalib1.mp4
	echo "Use flowblade movie editor."

# DQS=DroneQuarterSecond movie where drone makes its first appearance
# low into the right edge. That movie edited in flowblade
# from DroneCalib1.mp4 has 12 frames, with redundancies.

# We found ffmpeg dropped half the frames when it did mp4->bmp
# seq. conversion and
# 3/4 of them in the mp4 -> raw
# YUV movie.
# TestPipelineDQSToYUVsToBmps.sh writes the movie to a shell coproc pipe.
# See bash documentation and other resources for the bash coproc command.

DQS.mp4 : DroneCalib1.mp4
	echo "Only 797466B. Already in repository, checkout from GIT in needed."
	echo "Use flowblade movie editor."

# bash -c "<shell command>" is done to capture stdout/stderr messages
# It seems the bare command in a make recipe created the target file
# but stdout/err weren't redirected to it in its descendent processes.

DQS.rawMovie.yuv : DQS.mp4
	bash -c "ffmpeg -hide_banner -an -y -i DQS.mp4 DQS.rawMovie.yuv &>>DQS.rawMovie.yuv.messages"

#rawMovie.yuv means it's the concatenation of YUV frames, with no headers.
#FYI:Since YUVs have no headers, their dimension and format (like I420p) must be specified separately.

DQS.fmg.*.bmp : DQS.rawMovie.yuv
	bash -c "ffmpeg -hide_banner -an -y -i DQS.mp4 DQS.ffmpeg.%02d.bmp &>>DQS.fmg.bmp.messages"

# NOTE We take advantage of ffmpeg's writing of file sequences ---^^^^^^^^
# That feature doesn't seem to work when outputing to a ...%0d.yuv target.

DQS.fmgPipe* : DQS.mp4 TestPipelineDQSToYUVsToBmps.sh
	bash -c "TestPipelineDQSToYUVsToBmps.sh &>> DQS.fmgPipe.messages"


UnivTest.bmp : UnivI420pUnc4096x4096.yuv UnivI420pUnc4096x4096.bmp Makefile YUVToBMPStreamFilter
	YUVToBMPStreamFilter 4096 4096 < UnivI420pUnc4096x4096.yuv > UnivTest.bmp



clean:
	rm -f DQS.ffmpeg.*.bmp DQS.fmgPipe.*.bmp DQS.fmgPipe.*.yuv PIPE.yuv *.messages
	touch DQS.mp4

.PHONY: clean
