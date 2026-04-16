#CXXFLAGS = -Wno-unused-result  -g -lduma
CXXFLAGS = -DNDEBUG -Wno-unused-result -O3

# duma library https://github.com/johnsonjh/duma
# Detect Unintended Memory Access
# package in Ubuntu Linux

#CFLAGS   = -Wno-unused-result  -g -lduma
CFLAGS = -DNDEBUG -Wno-unused-result -O3
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


2x2.yuv : 2x2yuv Makefile
	2x2yuv > 2x2.yuv

2x2yuv : 2x2yuv.c Makefile
	$(CC) 2x2yuv.c -o 2x2yuv

