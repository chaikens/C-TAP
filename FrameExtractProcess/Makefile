all: yuvSelectMulti YUVToBMPStreamFilter

CXXFLAGS = -O3
#CXXFLAGS = -g

yuvSelectMulti: yuvSelectMulti.cpp bmp.h bmp.o yuvtobmpT.o yuvtobmpT.h UnivI420pUnc4096x4096.o
	$(CXX) $(CXXFLAGS) -Wno-format-security bmp.o UnivI420pUnc4096x4096.o yuvtobmpT.o  $< -o $@

YUVToBMPStreamFilter : YUVToBMPStreamFilter.cpp bmp.h bmp.o yuvtobmpT.o yuvtobmpT.h UnivI420pUnc4096x4096.o
	$(CXX) $(CXXFLAGS) bmp.o UnivI420pUnc4096x4096.o yuvtobmpT.o $< -o $@

UnivI420pUnc4096x4096.yuv : MakeUnivI420pUnc.cpp
	$(CXX) $(CXXFLAGS)  MakeUnivI420pUnc.cpp
	./a.out
	rm a.out
	-YUView UnivI420pUnc4096x4096.yuv &


UnivI420pUnc4096x4096.bmp  : UnivI420pUnc4096x4096.yuv
	ffmpeg -hide_banner -hide_banner -loglevel quiet -y -an -video_size 4096x4096 -i $< -frames:v 1 $@

UnivI420pUnc4096x4096.o : UnivI420pUnc4096x4096.s UnivI420pUnc4096x4096.bmp
	$(AS) UnivI420pUnc4096x4096.s -o $@ 

bmp.o : bmp.cpp bmp.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

yuvtobmpT.o : yuvtobmpT.h yuvtobmpT.cpp
	$(CXX) $(CXXFLAGS) -c  yuvtobmpT.cpp
