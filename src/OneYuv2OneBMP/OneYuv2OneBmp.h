#include "mustbelittleendian.h" //only developed for g++ little endian (like Linux PCs)

#if defined __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#endif
/*
 * gives us uint8_t uint16_t for our image and video files
 */

/*
Julien (surname unknown) suggests that there are problems with the above formulae and suggests the following instead:

    Y = 0.299R + 0.587G + 0.114B

    U'= (B-Y)*0.565

    V'= (R-Y)*0.713

with reciprocal versions:

    R = Y + 1.403V'

    G = Y - 0.344U' - 0.714V'

    B = Y + 1.770U'

from
https://fourcc.org/fccyvrgb.php
*/

inline void convert4toBMP(uint8_t *firstBMPPair, uint8_t *secondBMPPair,
	      const uint8_t *firstYPair, const uint8_t *secondYPair,
	      const uint8_t *Ubyte, const uint8_t *Vbyte)
{
  // dont bother hand optimizing: Max Compiler+CPU optimization should be great.
  //
  //order of colors in each BMP pixel:
  const int b = 0; const int g = 1; const int r = 2;

  // conversion matrix acts from the left:
  const float by = 1.0; const float bu = 1.770; const float bv = 0.0; const float b1 = 0.0;
  const float gy = 1.0; const float gu = -0.344; const float gv = -0.714; const float g1 = 0.0;
  const float ry = 1.0; const float ru = 0.0; const float rv = 1.403; const float r1 = 0.0;

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

/* YUVin points to the YUV data in memory.
   BMPout points to memory already allocated for the conversion a bmp file,
   including the 54 byte header */

  
