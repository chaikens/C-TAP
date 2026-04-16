TestOneYuv2OneBmp: OneYuv2OneBmp.o TestOneYuv2OneBmp.cpp OneYuv2OneBmp.h
	g++  TestOneYuv2OneBmp.cpp OneYuv2OneBmp.o -o TestOneYuv2OneBmp

OneYuv2OneBmp.o:  OneYuv2OneBmp.cpp OneYuv2OneBmp.h
	gcc  -c OneYuv2OneBmp.cpp

install: OneYuv2OneBmp.o
	cp OneYuv2OneBmp.o OneYuv2OneBmp.h ../..

