/**
   MakeUnivI420pUnc
   $ ./MakeUnivI420pUnc
   writes file UnivI420pUnc4096x4096.yuv into your working dir.

   When image is converted to a GRB bitmap, by say,

   $ ffmpeg -hide_banner -y -an -video_size 4096x4096 \
   -i UnivI420pUnc4096x4096.yuv -frames:v 1	      \
   UnivI420pUnc4096x4096.bmp 

   the resulting .bmp will be a table for converting
   YUV I420p images to a .bmp same way as what your .bmp
   converter did if it had used a functional YUV->BGR map.

   See ffmpeg.YUVtoBmp.sh

   No clipping to restrict the [0,255] range is done.

   This might be harmless if the YUVs you're converting
   are clipped to legal YUV values.

   UnivI420pUnc4096x4096.yuv has this structure, where byte
   sequences described as squares are in the file in row
   major order:  1234/5678 = 12345678. The YUV arrays are concatenated.
   Y: (256*16)x(256*16) square arrangement of equal 16x16 blocks
      Each block contains 0,1,2,...,15;16,...,31;... ...,247;248,249,...254,255 
   U: (256*8)x(256*8) square where each of the 256 groups of 8 COLUMNS
      contain identical byte values, 0 for the first group, 1 for the 2nd,
      ...  255 for the last.
   V: (256*8)x(256*8) square where each of the 256 groups of 8 ROWS
      contain identical byte values, 0 for the first group, 1 for the 2nd,
*/

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <errno.h>
#include <error.h>
 
int main(int argc, char *argv[])
{ char fname[] = "UnivI420pUnc4096x4096.yuv";
  FILE* fp = fopen(fname, "w");
  uint8_t *UYUV = new uint8_t[(4096*4096*3)/2];
  if( !UYUV ) {
    error(1, errno, "Mem alloc failure.");
  }
  uint8_t *Ys = UYUV;
  uint8_t *Us = UYUV+4096*4096;
  uint8_t *Vs = Us + 4096*4096/4;
  for(unsigned int V = 0; V < 256; V++)
    {
      for(unsigned int U = 0; U < 256; U++)
	{
	  for(unsigned int Y = 0; Y < 256; Y++)
	    {
	      size_t px = 16*U + (Y & 0xF);//must be 15=0xF=00001111, NOT 0xFF dum.
	      size_t py = 16*V + (Y >>  4);
	      Ys[4096*py + px] = Y;
	      Us[(4096/2)*(py/2) + (px/2) ] = U;
	      Vs[(4096/2)*(py/2) + (px/2) ] = V;
	    }
	}
    }
  if( 1 != fwrite(UYUV, (4096*4096*3)/2, 1, fp))
    {
      error(1, errno, "File writing failure.");
    }
  return 0;
}


     
