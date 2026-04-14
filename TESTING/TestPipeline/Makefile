
PATH := .:$(PATH)
bmps  := $(wildcard CATS/D*.bmp)
yuvs  := $(wildcard CATS/D*.yuv)
nbmp  := $(sh -c "echo $$(bmps)" | wc -c)
nyuv  := $(sh echo $$(yuvs) | wc -c)

all : CAT.bmp CAT.yuv

CAT.bmp : $(bmps)
	cat $(bmps) > CATS/CATS$(nbmp).bmp
	#NCATS=$(sh -c echo $$(bmps) | wc -w); echo $(NCATS); cat $(bmps) > CATS/CATS$(NCATS).bmp
	#echo cant get it to count the cats $(NCATS)

CAT.yuv : $(yuvs)
	cat $(yuvs) > CATS/CATS$(nbmp).yuv
	#NCATS=$(sh -c echo $(yuvs) | wc -w); echo $(NCATS); cat $(yuvs) > CATS/CATS$(NCATS).yuv
	#echo cant get it to count the cats $(NCATS)

CATS2.bmp : CATS/CATS.bmp
	cat CATS/CATS.bmp CATS/CATS.bmp > CATS/CATS2.bmp

TestPipeFromBmpFile :
	Phase1aPipe 0 10 < CATS/CATS2.bmp > CATS2.bmp.result
	echo See Our Result
	cat CATS2.bmp.result
