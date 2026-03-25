/*
Since this is for a first step in making 1a not rely on .bmp disk files,
we will hand the task off to ffmpeg via files.

If we continue to run 1a on bmp data, a more efficient soln may be better.
 */


#include <cstdint>
/*
 * gives us uint8_t uint16_t for our image and video files
 */
#include <cassert>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <error.h>
#include <cstdio>
#include <unistd.h>
#include <string.h>
#include <cmath>
#include "OneYuv2OneBmp.h"
#include <cstdlib> //for abs()

using namespace std;

//
//
// Let size=width*height.
// storeYsInBmp( width, height, YArray, pBMbytes);
//  Temporarily store size Y values in the 2nd byte of each of size rgb triples.
//
// storeUsInBmp( width, height, UArray, pBMbytes);
//    with full yuv in mem at YArray, ^---= YArray + width*height
//  Temporarily store given size/4 U values in 1st byte of each of size rgb triples. 
//
// useVsFinishBmp( width, height, VArray, pBMbytes);
//    and then also                   ^--== YArray + width*height+(1/4)*width*height 
//  Use the stored Ys and Us, and the last size/4 V values,
//  to compute the size rgb BMP triples were are converting to.
//
// This sequence reads the bytes of the I420p YUV raw image SEQUENTIALLY,
// supporting a pipeline to process a sequence of raw image frames.
//
//
static inline void storeYsInBmp( int width, int height,
				const uint8_t *pYs, uint8_t *pBMbytes
				) {
  width = abs(width); height = abs(height);
  for (int i = 0; i < width*height; i++) {
    //i indexes Y values
    pBMbytes[ 3*i + 1 ] = pYs[i];  //one green rgb[1] val per Y value 
  }
  return;
}

static inline void storeUsInBmp( int width, int height,
				const uint8_t *pUs, uint8_t *pBMbytes
				) {
  width = abs(width); height = abs(height);
  int halfwidth = width/2;
  int halfheight = height/2;
  assert((width == 2*halfwidth) && (height == 2*halfheight) );
  for (int i = 0; i < width*height/2; i++) {
    pBMbytes[ 3*2*i + 0 ] =
      pBMbytes[ 3*2*i + 3 ] =
      pBMbytes[ 3*(2*i + width) + 0 ] =
      pBMbytes[ 3*(2*i + width) + 3 ] = pUs[i];
  }
  return;
}

typedef struct rgbtriple { uint8_t tr; uint8_t tg; uint8_t tb; } rgbtriple;

static inline uint8_t uint8clamp( float x )
/* Please check if stricter clamping is needed */
{
  if( x < 0 )
    { return 0;}
  else {
    if(x > 255){return 255;}
    else {
      return (uint8_t) round( x );
    }
  }
}

#include "yuv2bmpConstants.cpp"
  // defines b, g, r and {b,g,r}{y,u,v,1} constants

static inline rgbtriple tripleFromYUV( uint8_t y, uint8_t u, uint8_t v )
{
  rgbtriple ANS;
  ANS.tr =  uint8clamp  (ry*y + ru*u + rv*v + r1);
  ANS.tg =  uint8clamp  (gy*y + gu*u + gv*v + g1);
  ANS.tb =  uint8clamp  (by*y + bu*u + bv*v + b1);
  return ANS;
}

static inline void useVsFinishBmp( int width, int height,
				   const uint8_t *pVs, uint8_t *pBMbytes)
{
  rgbtriple T;
  for (int i = 0; i < width*height/4; i++) {

    uint8_t commonU = pBMbytes[3*2*i];
    uint8_t commonV = pVs[i];

    // calc T
    T = tripleFromYUV( pBMbytes[3*2*i + 1], commonU, commonV); 
    pBMbytes[ 3*2*i + r ] = T.tr;
    pBMbytes[ 3*2*i + g ] = T.tg;
    pBMbytes[ 3*2*i + b ] = T.tb;

    // calc T
    T = tripleFromYUV( pBMbytes[3*2*i + 3 + 1], commonU, commonV); 
    pBMbytes[ 3*2*i + 3 + r ] = T.tr;
    pBMbytes[ 3*2*i + 3 + g ] = T.tg;
    pBMbytes[ 3*2*i + 3 + b ] = T.tb;

    // calc T
    T = tripleFromYUV( pBMbytes[3*2*i + 3*width + 1], commonU, commonV);
    pBMbytes[ 3*2*i + 3*width + r ] = T.tr;
    pBMbytes[ 3*2*i + 3*width + g ] = T.tg;
    pBMbytes[ 3*2*i + 3*width + b ] = T.tb;

    // calc T
    T = tripleFromYUV( pBMbytes[3*2*i + 3*width + 3 + 1], commonU, commonV);
    pBMbytes[ 3*2*i + 3*width + 3 + r ] = T.tr;
    pBMbytes[ 3*2*i + 3*width + 3 + g ] = T.tg;
    pBMbytes[ 3*2*i + 3*width + 3 + b ] = T.tb;
 
  }
  return;
}





int OneYuv2OneBmp(unsigned int width, unsigned int height,
		     const uint8_t *YUVin, uint8_t *BMPout)
{ //
  // Use ffmpeg and two temporary files.
  //
  system("rm -f OneTemp.yuv OneTemp.bmp");
  //printf("Will open OneTemp.yuv\n");
  int yuvFD = open("OneTemp.yuv", O_CREAT | O_RDWR, S_IRUSR|S_IWUSR);
  if( yuvFD < 0 )
    {
      error(1, errno, "Open OneTemp.yuv failed.");
    }
  size_t yuvlen = ( width * height * 3 ) / 2;
  size_t yuvwriteret = write(yuvFD, YUVin, yuvlen);
  assert(yuvlen == yuvwriteret);
  
  /* construct ffmpeg call like
ffmpeg -hide_banner -an -video_size 3840x2160 -i OneTemp.yuv -frames:v 1 OneTemp.bmp > ffmpeg.messages 2>&1 
  Only the size is variable. 
  */
  char command[240];
  command[0] = 0;
  const char *comPrefix = "ffmpeg -hide_banner -an -video_size ";
  strncat(command, comPrefix, strlen(comPrefix));
  char vidsizestr[12];
  int sizelen = sprintf(vidsizestr, "%ux%u", width, height);
  strncat(command, vidsizestr, sizelen);
  const char *comSuffix = " -i OneTemp.yuv -frames:v 1 OneTemp.bmp > OneYuvs.ffmpeg.msgs 2>&1";
  /* bash &> for redir both stdout and stderr didn't work */
  strncat(command, comSuffix, strlen(comSuffix));

  system(command);

  int bmpFD = open("OneTemp.bmp", O_RDONLY, 0);

  uint8_t *p = BMPout;
  size_t bmpreadret = read(bmpFD, p, yuvlen*2+54);
  if( bmpreadret < 0 )
    {
      error(1, errno, "Read of OneTemp.bmp failed.");
    }
  size_t readcount = bmpreadret ;
  while ( readcount < yuvlen*2+54 )
    {
      fprintf(stderr, "error, or just short Read of OneTemp.bmp, we did read %ld bytes.\n", readcount);
      if( bmpreadret < 0 )
	{
	  error(1, errno, "Read of OneTemp.bmp failed.");
	}
      readcount += bmpreadret;
      p += bmpreadret;
      bmpreadret = read(bmpFD, p, yuvlen*2+54-readcount);
    }
  system("rm -f OneTemp.bmp OneTemp.yuv");
  close(bmpFD);
  close(yuvFD);
  return 0;
}


int OneYuv2BmpDataFast(unsigned int width, unsigned int height,
		  const uint8_t *YUVin, uint8_t *BMdata)
  /* YUVin points to the YUV I420p data in memory.
   BMdata points to memory for the bitmap byte triples.
  */
{
//
//
// Let size=width*height.
  storeYsInBmp( (int) width, (int) height, YUVin, BMdata);
//  Temporarily store size Y values in the 2nd byte of each of size rgb triples.
//
  storeUsInBmp( (int) width, (int) height, YUVin+width*height, BMdata);
//    with full yuv in mem at YArray,         ^---= YArray + width*height
//  Temporarily store given size/4 U values in 1st byte of each of size rgb triples. 
//
  useVsFinishBmp( (int) width, (int) height, YUVin+width*height+(width/2)*(height/2), BMdata);
//    and then also                   ^--== YArray + 3*width*height+(1/4)*width*height 
//  Use the stored Ys and Us, and the last size/4 V values,
//  to compute the size rgb BMP triples were are converting to.
//
// This sequence reads the bytes of the I420p YUV raw image SEQUENTIALLY,
// supporting a pipeline to process a sequence of raw image frames.
//
//
  return 0;
}
  


