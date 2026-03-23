TestOneYuv2OneBmp: OneYuv2OneBmp.o TestOneYuv2OneBmp.cpp OneYuv2OneBmp.h Makefile
	g++  -g TestOneYuv2OneBmp.cpp OneYuv2OneBmp.o -o TestOneYuv2OneBmp

OneYuv2OneBmp.o:  OneYuv2OneBmp.cpp OneYuv2OneBmp.h Makefile
	g++  -g -c OneYuv2OneBmp.cpp


SeeOptconvert4toBMP.txt : OneYuv2OneBmp.h SeeOptconvert4toBMP.cpp Makefile
	g++ -g -O3 -Wno-unused-result -c SeeOptconvert4toBMP.cpp
	objdump --reloc --disassemble --source SeeOptconvert4toBMP.o > SeeOptconvert4toBMP.txt
	less SeeOptconvert4toBMP.txt

analyzeBMP : analyzeBMP.cpp bmp.h mustbelittleendian.h Makefile
	g++ -g analyzeBMP.cpp -o analyzeBMP

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


Test: TestOneFrame.yuv TestOneYuv2OneBmp Makefile
	./TestOneYuv2OneBmp 3840x2160 TestOneFrame.yuv TestResult.bmp
	eog TestResult.bmp


TestOneFrame.yuv :
	ffmpeg -hide_banner -y -i ../../DroneCalib1.mp4 -frames:v 1  TestOneFrame.yuv
