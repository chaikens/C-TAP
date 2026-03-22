TestOneYuv2OneBmp: OneYuv2OneBmp.o TestOneYuv2OneBmp.cpp OneYuv2OneBmp.h
	g++  TestOneYuv2OneBmp.cpp OneYuv2OneBmp.o -o TestOneYuv2OneBmp

OneYuv2OneBmp.o:  OneYuv2OneBmp.cpp OneYuv2OneBmp.h
	gcc  -c OneYuv2OneBmp.cpp


SeeOptconvert4toBMP.txt : OneYuv2OneBmp.h SeeOptconvert4toBMP.cpp Makefile
	gcc -g -O3 -Wno-unused-result -c SeeOptconvert4toBMP.cpp
	objdump --reloc --disassemble --source SeeOptconvert4toBMP.o > SeeOptconvert4toBMP.txt
	less SeeOptconvert4toBMP.txt


install: OneYuv2OneBmp.o
	cp OneYuv2OneBmp.o OneYuv2OneBmp.h ../..

