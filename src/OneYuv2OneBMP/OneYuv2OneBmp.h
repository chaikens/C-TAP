#include "mustbelittleendian.h" //only developed for g++ little endian (like Linux PCs)

#include "bmp.h"

#if defined __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#endif
/*
 * gives us uint8_t uint16_t for our image and video files
 */

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
				const uint8_t *pYs, uint8_t *pBMbytes);
static inline void storeUsInBmp( int width, int height,
				const uint8_t *pUs, uint8_t *pBMbytes);
static inline void useVsFinishBmp( int width, int height,
				   const uint8_t *pVs, uint8_t *pBMbytes);

inline void convert4toBMP(uint8_t *firstBMPPair, uint8_t *secondBMPPair,
	      const uint8_t *firstYPair, const uint8_t *secondYPair,
	      const uint8_t *Ubyte, const uint8_t *Vbyte)
{
  // dont bother hand optimizing: Max Compiler+CPU optimization should be great.
  //
  
#include "yuv2bmpConstants.cpp"
  //defines b, g, r and {b,g,r}{y,u,v,1} act-from-left conversion constants
  
  // contributions from U and V common to our 4 pixels:
  float buv = bu*Ubyte[0] + bv*Vbyte[0] + b1;
  float guv = gu*Ubyte[0] + gv*Vbyte[0] + g1;
  float ruv = ru*Ubyte[0] + rv*Vbyte[0] + r1;

#define f2rgb( x ) (uint8_t ( x ))
  
  firstBMPPair[b] = f2rgb(buv + by*firstYPair[0]);
  firstBMPPair[g] = f2rgb(guv + gy*firstYPair[0]);
  firstBMPPair[r] = f2rgb(ruv + ry*firstYPair[0]);
  firstBMPPair[b+3] = f2rgb(buv + by*firstYPair[1]);
  firstBMPPair[g+3] = f2rgb(guv + gy*firstYPair[1]);
  firstBMPPair[r+3] = f2rgb(ruv + ry*firstYPair[1]);

  secondBMPPair[b] = f2rgb(buv + by*secondYPair[0]);
  secondBMPPair[g] = f2rgb(guv + gy*secondYPair[0]);
  secondBMPPair[r] = f2rgb(ruv + ry*secondYPair[0]);
  secondBMPPair[b+3] = f2rgb(buv + by*secondYPair[1]);
  secondBMPPair[g+3] = f2rgb(guv + gy*secondYPair[1]);
  secondBMPPair[r+3] = f2rgb(ruv + ry*secondYPair[1]);

#undef f2rgb
  
}

int OneYuv2OneBmp(unsigned int width, unsigned int height,
		  const uint8_t *YUVin, uint8_t *BMPout);
int OneYuv2OneBmpFast(unsigned int width, unsigned int height,
		  const uint8_t *YUVin, uint8_t *BMPout);
int OneYuv2BmpDataFast(unsigned int width, unsigned int height,
		       const uint8_t *YUVin, uint8_t *BMdata);
  /* YUVin points to the YUV I420p data in memory.
   BMdata points to memory for the bitmap byte triples.
  */


  
