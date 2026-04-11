all: depend
.PHONY : all

progs : random_image TestOneYuv2OneBmp analyzeBMP 
.PHONY : progs

CXXFLAGS = -Wno-unused-result  -g -lduma
#CXXFLAGS = -DNDEBUG -Wno-unused-result -O3
CFLAGS   = -Wno-unused-result  -g
#CFLAGS = -DNDEBUG -Wno-unused-result -O3
PATH     := .:$(PATH)


4x2.yuv : 4x2yuv.c
	$(CC) $(CFLAGS) $< -o $@.exe
	$@.exe > $@

16x12.yuv : 16x12yuv.c
	$(CC) $(CFLAGS) $< -o $@.exe
	$@.exe > $@

48x36.yuv : 48x36yuv.c
	$(CC) $(CFLAGS) $< -o $@.exe
	$@.exe > $@

6x6.yuv : 6x6yuv.c
	$(CC) $(CFLAGS) $< -o $@.exe
	$@.exe > $@


random_image: random_image.cpp bmp.o
	$(CXX) $(CXXFLAGS)  $^ -o $@

bmp.o : bmp.cpp bmp.h Makefile
	$(CXX) $(CXXFLAGS) -c $<

TestOneYuv2OneBmp: bmp.o OneYuv2OneBmp.o TestOneYuv2OneBmp.cpp OneYuv2OneBmp.h Makefile
	$(CXX) $(CXXFLAGS) TestOneYuv2OneBmp.cpp OneYuv2OneBmp.o bmp.o -o TestOneYuv2OneBmp

OneYuv2OneBmp.o:  OneYuv2OneBmp.cpp OneYuv2OneBmp.h bmp.h Makefile
	$(CXX) $(CXXFLAGS) -c OneYuv2OneBmp.cpp


YUVToBMPStreamFilter : I420pRef4096x4096Frame.bmp YUVToBMPStreamFilter.cpp OneYuv2OneBmp.h bmp.h OneYuv2OneBmp.o bmp.o Makefile
	$(CXX) $(CXXFLAGS) YUVToBMPStreamFilter.cpp bmp.o OneYuv2OneBmp.o -o YUVToBMPStreamFilter

TestYUVToBMPStreamFilter : YUVToBMPStreamFilter
	ulimit -c unlimited; ./YUVToBMPStreamFilter --fd 3 \
            4096 4096 \
            3< I420pRef4096x4096Frame.yuv \
            > TestYUVToStreamFilter.bmp
	eog TestYUVToStreamFilter.bmp

TestTinyYUVToBMPStreamFilter : YUVToBMPStreamFilter 2x2.yuv Makefile
	ulimit -c unlimited; ./YUVToBMPStreamFilter --fd 3 \
             2 2 \
             3< 2x2.yuv \
             > TestTinyYUVToStreamFilter.bmp

2x2.yuv : 2x2yuv Makefile
	2x2yuv > 2x2.yuv

2x2yuv : 2x2yuv.c Makefile
	$(CC) 2x2yuv.c -o 2x2yuv

SeeOptconvert4toBMP.txt : OneYuv2OneBmp.h SeeOptconvert4toBMP.cpp Makefile
	$(CXX) $(CXXFLAGS) -O3 -Wno-unused-result -c SeeOptconvert4toBMP.cpp
	objdump --reloc --disassemble --source SeeOptconvert4toBMP.o > SeeOptconvert4toBMP.txt
	less SeeOptconvert4toBMP.txt

analyzeBMP : analyzeBMP.cpp bmp.h mustbelittleendian.h Makefile
	$(CXX) $(CXXFLAGS) analyzeBMP.cpp -o analyzeBMP

CTAPsBPManalysis.txt: analyzeBMP Makefile
	echo "in Dir " > CTAPsBMPanalysis.txt
	pwd >> CTAPsBMPanalysis.txt
	echo >> CTAPsBMPanalysis.txt
	echo "../../bitmaps/thumb000001.bmp" >> CTAPsBMPanalysis.txt
	echo >> CTAPsBMPanalysis.txt
	./analyzeBMP < ../../bitmaps/thumb000001.bmp >> CTAPsBMPanalysis.txt
	echo >> CTAPsBMPanalysis.txt
	echo "../../bitmaps/pic00038.bmp" >> CTAPsBMPanalysis.txt
	echo >> CTAPsBMPanalysis.txt
	./analyzeBMP < ../../bitmaps/pic000038.bmp >> CTAPsBMPanalysis.txt

install: OneYuv2OneBmp.o
	cp OneYuv2OneBmp.o OneYuv2OneBmp.h ../..
.PHONY : install

Test: TestOneFrame.yuv TestOneYuv2OneBmp Makefile
	-./TestOneYuv2OneBmp 3840x2160 TestOneFrame.yuv OldTestResult.bmp
	-./TestOneYuv2OneBmp --table 3840x2160 TestOneFrame.yuv TableTestResult.bmp
	eog OldTestResult.bmp TableTestResult.bmp
.PHONY : Test

makeBigReferenceYUV: makeBigReferenceYUV.cpp OneYuv2OneBmp.o Makefile
	$(CXX) $(CXXFLAGS) makeBigReferenceYUV.cpp OneYuv2OneBmp.o -o makeBigReferenceYUV


TestOneFrame.yuv : Makefile
	ffmpeg -hide_banner -y -i ../../DroneCalib1.mp4 -frames:v 1  TestOneFrame.yuv

testbmp : testbmp.cpp bmp.o bmp.h Makefile
	$(CXX) $(CXXFLAGS)  $< bmp.o -o $@
	./testbmp

referenceBmp referenceBmp.bmp: referenceBmp.cpp bmp.o bmp.h Makefile
	$(CXX) $(CXXFLAGS) $< bmp.o -o $@
	-./referenceBmp
	-eog referenceBmp.bmp

referenceBmp_ffmpeg.yuv : referenceBmp.bmp Makefile
	ffmpeg -hide_banner -y -i referenceBmp.bmp -pix_fmt yuv420p referenceBmp_ffmpeg.yuv &>referenceBmp_bmp_toyuv_ffmpeg.txt

referenceBmp_yuv_ffmpeg.bmp : referenceBmp_ffmpeg.yuv TestOneYuv2OneBmp Makefile
	./TestOneYuv2OneBmp 64x32 referenceBmp_ffmpeg.yuv referenceBmp_yuv_ffmpeg.bmp 

referenceBmp_yuv_CTAP.bmp : referenceBmp_ffmpeg.yuv TestOneYuv2OneBmp Makefile
	./TestOneYuv2OneBmp --fast 64x32 referenceBmp_ffmpeg.yuv referenceBmp_yuv_CTAP.bmp

I420pRef4096x4096Frame.yuv : makeBigReferenceYUV Makefile
	makeBigReferenceYUV

I420pRef4096x4096Frame.bmp : I420pRef4096x4096Frame.yuv  Makefile
	ffmpeg -hide_banner -y -an -video_size 4096x4096 -i I420pRef4096x4096Frame.yuv -frames:v 1 I420pRef4096x4096Frame.bmp

depend:
	-gccmakedep *.cpp 
	echo "For now, give specific targets to make"
	echo
.PHONY : depend

#dependencies found by gccmakedep after removing external ones.

JuliensOneYuv2OneBmp.o: JuliensOneYuv2OneBmp.cpp

OneYuv2OneBmp.o: OneYuv2OneBmp.cpp mustbelittleendian.h bmp.h yuv2bmpConstants.cpp 

SeeOptconvert4toBMP.o: SeeOptconvert4toBMP.cpp OneYuv2OneBmp.h mustbelittleendian.h bmp.h

TestOneYuv2OneBmp.o: TestOneYuv2OneBmp.cpp  mustbelittleendian.h bmp.h

TryMacro.o: TryMacro.cpp

TrymacroExpanded.o: TrymacroExpanded.cpp

YUVToBMPStreamFilter.o: YUVToBMPStreamFilter.cpp OneYuv2OneBmp.h mustbelittleendian.h bmp.h 

analyzeBMP.o: analyzeBMP.cpp  bmp.h mustbelittleendian.h

bmp.o: bmp.cpp  bmp.h mustbelittleendian.h

makeBigReferenceYUV.o: makeBigReferenceYUV.cpp 

random_image.o: random_image.cpp  bmp.h mustbelittleendian.h

referenceBmp.o: referenceBmp.cpp bmp.h  mustbelittleendian.h

testbmp.o: testbmp.cpp  bmp.h  mustbelittleendian.h

yuv2bmpConstants.o: yuv2bmpConstants.cpp

# DO NOT DELETE

