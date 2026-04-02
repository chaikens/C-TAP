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
#include <iostream>

using namespace std;

//
//
// Let size=width*height.
// storeYsInBmp( width, height, YArray, pBMbytes);
//  Temporarily store size Y values in the 2nd byte of each of size rgb triples.
//
// storeUsInBmp( width, height, UArray, pBMbytes);
//    with full yuv in mem at YArray, ^---= YArray + width*height
//  Temporarily store given size/4 U values, each 4 times, each time
//  in the 1st byte of a rgb triple.  The 4 triples correspond to a
//  2x2 square block of pixels.
//
// useVsFinishBmp( width, height, VArray, pBMbytes);
//    and then also                   ^--== YArray + width*height+(1/4)*width*height 
//  Use the stored Ys and Us, and the last size/4 V values,
//  to finish computing the BMP triples.  The V values, and corresponding
//  4 Ys and (common) Us are processed in the same order as the preious step.
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

/* This macro was to be used twice below but the compiler didn't like it
   I tried to be clever by substituting code into code */

#define UVInBmpMainLoop( Save12, Save34 ) \
  { int nPixRowsToGo = height; /*Count down.  We go down two at a time*/ \
  /*  until we get to 0 and are done or 1 when we do the one last row.*/ \
  while( nPixRowsToGo > 0 )                                              \
    {                                                                    \
      if( nPixRowsToGo != 1 ) {                                          \
	for( int iUV = 0; iUV <= halfwidth iUV++ ) {                     \
	  /* 4 saves: */                                                 \
         { Save12 }                                                      \
         { Save34 }                                                      \
	}                                                                \
	nPixRowsToGo -= 2;                                               \
      }                                                                  \
      else {                                                             \
	for( int iUV = 0; iUV <= halfwidth iUV++ ) {                     \
	  /* only 2 saves: */                                            \
	  { Save12 }						         \
	  nPixRowsToGo -= 1;                                             \
	}                                                                \
      }                                                                  \
    } }
	  
/* End of macro def'n */

static inline void storeUsInBmp( int width, int height,
				const uint8_t *pUs, uint8_t *pBMbytes
				) {
  width = abs(width); height = abs(height);
  int halfwidth = width/2;
  // Do all yuv images have even lengths and heights?
  // Maybe look this up.
  // For now, we'll tolerate an odd number height of rows.
  // In any case, maybe code to exit gracefully instead of crashing.
  assert((width == 2*halfwidth) );

  /* Macro hack doen't work, do by hand below:
  UVinBmpMainLoop ( {
		     pBMbytes[ 3*2*iUV + 0 ] =
		       pBMbytes[ 3*2*iUV + 3 ] = pUs[iUV];
		   },
		   {
		     pBMbytes[ 3*(2*iUV + width) + 0 ] =
		       pBMbytes[ 3*(2*iUV + width) + 3 ] = pUs[iUV];
		   }
		   )
		   */

  { int nPixRowsToGo = height; /*Count down.  We go down two at a time*/ 
  /*  until we get to 0 and are done or 1 when we do the one last row.*/ 
  while( nPixRowsToGo > 0 )                                              
    {                                                                    
      if( nPixRowsToGo != 1 ) {                                          
	for( int iUV = 0; iUV <= halfwidth; iUV++ ) {                     
	  /* 4 saves: */                                                 
         {
		     pBMbytes[ 3*4*iUV + 0 ] =
		       pBMbytes[ 3*4*iUV + 3 ] = pUs[iUV];
	 }
	 {
		     pBMbytes[ 3*(4*iUV + width) + 0 ] =
		       pBMbytes[ 3*(4*iUV + width) + 3 ] = pUs[iUV];
	 }
	}                                                                
	nPixRowsToGo -= 2;                                               
      }                                                                  
      else {                                                             
	for( int iUV = 0; iUV <= halfwidth; iUV++ ) {                     
	  /* only 2 saves: */                                            
	  {
	    pBMbytes[ 3*2*iUV + 0 ] =
		       pBMbytes[ 3*4*iUV + 3 ] = pUs[iUV];
	  }						         
	  nPixRowsToGo -= 1;                                             
	}                                                                
      }                                                             
    }
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
  width = abs(width); height = abs(height);
  int halfwidth = width/2;

  uint8_t commonU; //For our usually 4 pixels
  uint8_t commonV; //For our usually 4 pixels
  
  rgbtriple T;
  
  /*  Not only does the macro version not compile, but its application code below is WRONG. 
    UVinBmpMainLoop (
		   { // common{U,V} will be used in 2nd code arg.
		     commonU = pBMbytes[ 3*4*iUV ];
		     commonV = pVs[iUV];

		     T = tripleFromYUV( pBMbytes[3*2*iUV + 1], commonU, commonV); 
		     pBMbytes[ 3*2*iUV + r ] = T.tr;
		     pBMbytes[ 3*2*iUV + g ] = T.tg;
		     pBMbytes[ 3*2*iUV + b ] = T.tb;

		     T = tripleFromYUV( pBMbytes[3*2*iUV + 3 + 1], commonU, commonV); 
		     pBMbytes[ 3*2*iUV + 3 + r ] = T.tr;
		     pBMbytes[ 3*2*iUV + 3 + g ] = T.tg;
		     pBMbytes[ 3*2*iUV + 3 + b ] = T.tb;
		   },
		   {
		     T = tripleFromYUV( pBMbytes[3*2*iUV + 3*width + 1], commonU, commonV);
		     pBMbytes[ 3*2*iUV + 3*width + r ] = T.tr;
		     pBMbytes[ 3*2*iUV + 3*width + g ] = T.tg;
		     pBMbytes[ 3*2*iUV + 3*width + b ] = T.tb;

		     T = tripleFromYUV( pBMbytes[3*2*iUV + 3*width + 3 + 1], commonU, commonV);
		     pBMbytes[ 3*2*iUV + 3*width + 3 + r ] = T.tr;
		     pBMbytes[ 3*2*iUV + 3*width + 3 + g ] = T.tg;
		     pBMbytes[ 3*2*iUV + 3*width + 3 + b ] = T.tb;
		   }
		   )
  */
  int nPixRowsToGo = height; /*Count down.  We go down two at a time */ 
  /*  until we get to 0 and are done or 1 when we do the one last row.*/ 
  int iUV = 0; /* index into the V (for us here) uint8_t array in the YUV file*/
               /* We stored the correponding U values in the RGB array so that we
                  might process the Y, then U, then V values in a pipeline.  */
  int row = 0; /* pixel row we will process next, numbered from 0, also number of  rows finished.*/
  while( nPixRowsToGo > 0 )                                              
    {                                                                    
      if( nPixRowsToGo != 1 ) {                                          
	for( int x = 0; x <= width; x += 2 ) {
	  /* 4 saves: */                                                 
	  { //first of an adjacent pair of rows
	    int iByte0Pix = 3*width*row + 3*x;
	   // common{U,V} will be used in 2nd code arg.
	   commonU = pBMbytes[ iByte0Pix ];
	   commonV = pVs[iUV];

	   T = tripleFromYUV( pBMbytes[iByte0Pix + 1], commonU, commonV); 
	   pBMbytes[ iByte0Pix + r ] = T.tr;
	   pBMbytes[ iByte0Pix + g ] = T.tg;
	   pBMbytes[ iByte0Pix + b ] = T.tb;

	   T = tripleFromYUV( pBMbytes[iByte0Pix + 3 + 1], commonU, commonV); 
	   pBMbytes[ iByte0Pix + 3 + r ] = T.tr;
	   pBMbytes[ iByte0Pix + 3 + g ] = T.tg;
	   pBMbytes[ iByte0Pix + 3 + b ] = T.tb;
	 }
	  { //second of the adjacent pair of rows
	    int iByte0Pix = 3*width*(row+1) + 3*x;
	    T = tripleFromYUV( pBMbytes[iByte0Pix  + 1], commonU, commonV); 
	    pBMbytes[ iByte0Pix + r ] = T.tr;
	    pBMbytes[ iByte0Pix + g ] = T.tg;
	    pBMbytes[ iByte0Pix + b ] = T.tb;

	    T = tripleFromYUV( pBMbytes[iByte0Pix + 3 + 1], commonU, commonV); 
	    pBMbytes[ iByte0Pix + 3 + r ] = T.tr;
	    pBMbytes[ iByte0Pix + 3 + g ] = T.tg;
	    pBMbytes[ iByte0Pix + 3 + b ] = T.tb;
	 }
	iUV++;
	}
	row += 2;
	nPixRowsToGo -= 2;                                               
      }                                                                  
      else {
	for( int x = 0; x <= width; x += 2 ) {                     
	  /* 2 saves: */                                                 
	  { int iByte0Pix = 3*width*row + 3*x;
	   // common{U,V} will be used in 2nd code arg.
	   commonU = pBMbytes[ iByte0Pix ]; 
	   commonV = pVs[iUV];

	   T = tripleFromYUV( pBMbytes[iByte0Pix + 1], commonU, commonV); 
	   pBMbytes[ iByte0Pix + r ] = T.tr;
	   pBMbytes[ iByte0Pix + g ] = T.tg;
	   pBMbytes[ iByte0Pix + b ] = T.tb;

	   T = tripleFromYUV( pBMbytes[iByte0Pix + 3 + 1], commonU, commonV); 
	   pBMbytes[ iByte0Pix + 3 + r ] = T.tr;
	   pBMbytes[ iByte0Pix + 3 + g ] = T.tg;
	   pBMbytes[ iByte0Pix + 3 + b ] = T.tb;
	  }
	  iUV++;
	}
	row += 1;
	nPixRowsToGo -= 1;
      }
    }
  return ;
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
  //assert(yuvlen == yuvwriteret);
  
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
//    and then also                          ^--== YArray + width*height+(1/4)*width*height 
//  Use the stored Ys and Us, and the last size/4 V values,
//  to compute the size rgb BMP triples were are converting to.
//
// This sequence reads the bytes of the I420p YUV raw image SEQUENTIALLY,
// supporting a pipeline to process a sequence of raw image frames.
//
//
  return 0;
}

int MakeReferenceYUV()
{
/*************************************************************** 
 We compute our YUP I420p Universal Frame, then save or pipe it
 for ffmpeg to convert it to a Classical .bmp file.
 We'll have a function to compute from a YUV triple the pixel
 index corresponding the pixel corresponding to the YUV indices
 for the pixel in the .bmp file with color correpoinding to the
 YUV triple.

 Structure of the .bmp representation of the image, with annotations
 for the YUV indices and values. 
 (Bmps render upside-down images when the rows 0, 1, ... are rendered 
 top-to-bottom, so their x (row) and y (column) coordinates follow 
 mathematicians' 1st quadrant convention.)

     Rows;
       for iV, all 2^8 Vvalues,
           for iYh, all 2^4 high order 4-bit components of Yvalues
              compute a pixel row, row index is 2^4*iV + iYh;  
                           (to give a total of 2^12 rows);
                   The Y array indices are identical to pixel indices;

              retrieve, save for row processing (from the correct i? var) 
                a UorVrow, index iUorVrow every other pixel row

      ---------------------------------------------------------------

      Columns for each row;

              for iU, all 2^8 Uvalues,
                  for iHl, all 2^4 low order compontents of Yvalues,
                      retrieve (from the right i? var) and store the Y, U and V value,
                          whose pixel column index is 2*4*iU + iHl; 
			  a total 2^8*2^4 such triples;

                          The Y array indices are identical to pixel indices;
                      
                          use an UorVcol index, iUorVcol, 
                              one for every adjacent pair of pixel cols;



The Y array indices follow the pixel array indices
       iY = iP = 2^8*2^4*irow + icol

The U or V array is indexed to evenly located 2x2 blocks of pixels
(so U,V length is (1/4)*nPixels.


***************************************************************/ 

      
 
  const size_t pcols = 2<<12;  //number of pixel columns   
  const size_t rows  = 2<<12;  //number of pixel and byte rows
  
  const char outfilename[] = "I420pRef4096x4096Frame.yuv";




  size_t psize = pcols*rows;  //number of pixels

  
  uint8_t *Y = new uint8_t[psize + psize/4 + psize/4];
  if( Y == 0 )
    {
      cerr << "Failed memory alloc for our huge " <<
	(psize + psize/4 + psize/4) <<
	" byte array.";
      exit (1);
    }
  //byte array for Y followed by the rest of YUV data 

  uint8_t *U = Y + psize/4; //contains U array
  uint8_t *V = U + psize/4; //and V array
  
  int pixRowParity = 0;
  int iUandV = 0;

  int iPix = 0;

  for( int iV = 0; iV < 256; iV++ )
    { /*
       */
      for( int iHh = 0; iHh < 16; iHh++ )
	{ /*
	   */
	  for( int iU = 0; iU < 256; iU++ )
	    { /*
	       */
	      for(int iYl = 0; iYl < 16; iYl++)
		{ /*

		   */
		  // end of iYl loop  
		}
	      // end of iU loop
	    }
	  // end of iU loop
	}
      //end of iV loop
    }

  FILE* fp = fopen(outfilename, "w");
  if(!fp) {
    error(1, errno, "Output file %s opening failed.", outfilename);
  }
  size_t wret = fwrite( Y, 4096*3, 4096, fp);
  if( wret != 4096 )
    {
      error(1, errno, "Failed or short file %s writing.", outfilename);
    }

  int cret = fclose(fp);
  if(cret) {
    error(1, errno, "Closing of %s failed.", outfilename);
  }

  return 0;

}

/*
int main(int argc, char **argv)
{
  return MakeReferenceYUV();
}
*/
