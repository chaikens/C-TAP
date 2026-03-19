TestOneYuv2OneBmp: OneYuv2OneBmp.o TestOneYuv2OneBmp.c OneYuv2OneBmp.h
	gcc  -g TestOneYuv2OneBmp.c OneYuv2OneBmp.o -o TestOneYuv2OneBmp

OneYuv2OneBmp.o:  OneYuv2OneBmp.c OneYuv2OneBmp.h
	gcc  -g -c OneYuv2OneBmp.c

install: OneYuv2OneBmp.o
	cp OneYuv2OneBmp.o OneYuv2OneBmp.h ../..

