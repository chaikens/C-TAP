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


#define myerror(message) {fprintf(stderr, "OneYuv2OneBmp.cpp error: %s \n", message); assert(0); }


//for debugging bug of too-short yuv stdin stream when redirected from a file.
//#define FGETC(FP) (fgetcCount++, fgetc(FP)) 

unsigned int fgetcCount;
int FGETC(FILE* FP) {
  fgetcCount++;
  return fgetc(FP);
}

using namespace std;

//
//
// Let size=width*height.
// storeYsInBmp( width, height, YArray, pBMbytes);
//  Temporarily store size Y values in the 2nd byte of each of size bgr triples.
//
// storeUsInBmp( width, height, UArray, pBMbytes);
//    with full yuv in mem at YArray, ^---= YArray + width*height
//  Temporarily store given size/4 U values, each 4 times, each time
//  in the 1st byte of a bgr triple.  The 4 triples correspond to a
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
    pBMbytes[ 3*i + 1 ] = pYs[i];  //one green bgr[1] val per Y value 
  }
  return;
}

static inline void storeStreamYsInBmp( int width, int height,
				       FILE *YUVinFILE, uint8_t *pBMbytes, BMclass *pBM
				) {
  width = abs(width); height = abs(height);
  /*
  for (int i = 0; i < width*height; i++) {
    //i indexes Y values
    int Yval = FGETC(YUVinFILE);
    if(Yval == EOF) {
      myerror("storeStreamYsInBmp stream read EOF??");
    }
    //pBMbytes[ 3*i + 1 ]
    pBM->pD[3*i + 1] = (uint8_t) Yval;  //one green bgr[1] val per Y value 
  }
  */

  for(size_t irow = 0; irow < height; irow++)
    {
      uint8_t *prow = pBM->pR(irow);
      for(size_t iPixcol = 0; iPixcol < width; iPixcol++)
	{
	   int Yval = FGETC(YUVinFILE);
	   if(Yval == EOF) {
	     myerror("storeStreamYsInBmp stream read EOF??");
	   }
	   prow[3*iPixcol + 1] = (uint8_t) Yval;
	   //Store temporarilly in the green byte.
	}
    }
  
  assert(fgetcCount == width*height);
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
	for( int iUV = 0; iUV < halfwidth iUV++ ) {                     \
	  /* 4 saves: */                                                 \
         { Save12 }                                                      \
         { Save34 }                                                      \
	}                                                                \
	nPixRowsToGo -= 2;                                               \
      }                                                                  \
      else {                                                             \
	for( int iUV = 0; iUV < halfwidth iUV++ ) {                     \
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
  // Do all yuv images have even widths and heights?
  // Maybe look this up.
  // Microsoft .bmp image rows are 4-byte aligned.
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
	for( int iUV = 0; iUV < halfwidth; iUV++ ) {                     
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
	for( int iUV = 0; iUV < halfwidth; iUV++ ) {                     
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

static inline void storeStreamUsInBmp( int width, int height,
				       FILE * YUVinFILE, uint8_t *pBMbytes, BMclass *pBM)
{
  width = abs(width); height = abs(height);
  int halfwidth = width/2;
  // Do all yuv images have even widths and heights?
  // Maybe look this up.
  // Microsoft .bmp image rows are 4-byte aligned.
  // For now, we'll tolerate an odd number height of rows.
  // In any case, maybe code to exit gracefully instead of crashing.
  assert((width == 2*halfwidth) );

  int nPixRowsToGo = height; /*Count down.  We go down two at a time*/ 
  /*  until we get to 0 and are done or 1 when we do the one last row.*/
  
  int irow = 0; /*row number of pixel row to process = num rows finished.*/

  while( nPixRowsToGo > 0 )                                              
    { //Process the next pair of rows or the last 1 odd row.
      uint8_t *pRow = pBM->pR(irow);
      if( nPixRowsToGo != 1 )
	{ //Process the next pair of rows irow and irow+1
	  uint8_t *p2ndRow = pBM->pR(irow + 1);
	  //There are halfwidth Us used, each one for 2 bytes in each of the next 2 rows.
	  for( int iBInRowOfUs = 0; iBInRowOfUs < halfwidth; iBInRowOfUs++ )
	    {
	      //we don't use the index into the U array to retrieve U values
	      //since it is not in memory and its entries are read sequentially.
	      int Uval = FGETC(YUVinFILE);
	      if( Uval == EOF ) {
		myerror("Failed to get one U for 2 rows in storeStreamUsInBmp stream read EOF??");
	      }
	      /* 4 saves: */
	      
	      // pBMbytes[ 3*width*irow   /* 3*npixels above our row */ +
	      // 		3*2*iBInRowOfUs/* 3*npixels before the 1st pixel of our 2x2 block  */
	      // 		+ 0 /* save Uval in the blue pixel */ ] =              
	      // 	pBMbytes[ 3*width*irow +
	      // 		  3*2*iBInRowOfUs
	      // 		  + 3 /* first byte of the 2nd pixel*/
	      // 		  + 0 /* save Uval in the blue pixel */ ] = (uint8_t) Uval;

	      // //do the same for the next row after irow
	      // pBMbytes[ 3*width*(irow+1) +
	      // 		3*2*iBInRowOfUs 
	      // 		+ 0 ] =
	      // 	pBMbytes[ 3*width*(irow+1) +
	      // 		  3*2*iBInRowOfUs 
	      // 		  + 3
	      // 		  + 0 ] = (uint8_t) Uval;

	      pRow[3*2*iBInRowOfUs /* 3*npixels before the 1st pixel of our 2x2 block */
		   + 0 /* save Uval in the blue pixel byte */ ] =
		pRow[3*2*iBInRowOfUs + 3 + 0] = (uint8_t) Uval;

	      p2ndRow[3*2*iBInRowOfUs /* 3*npixels before the 1st pixel of our 2x2 block */
		      + 0 /* save Uval in the blue pixel byte */ ] =
		p2ndRow[3*2*iBInRowOfUs + 3 + 0] = (uint8_t) Uval;

	      //go on to the next Uval
	      }
	    //Finished one pair of rows. 
	    irow += 2;
	    nPixRowsToGo -= 2;
	  }                                                                  
	else
	  { //Process last one row, irow, of an odd number of rows.
	    //There are halfwidth Us used, each one for 2 bytes in each of the next 2 rows.
	    for( int iBInRowOfUs = 0; iBInRowOfUs < halfwidth; iBInRowOfUs++ )
	      {
		//we don't use the index into the U array to retrieve U values
		//since it is not in memory and its entries are read sequentially.
		int Uval = FGETC(YUVinFILE);
		if( Uval == EOF ) {
		  myerror("Failed to get one U for 2 rows in storeStreamUsInBmp stream read EOF??");
		}
		
		/* 2 saves: */                                                 

		// pBMbytes[ 3*width*irow   /* 3*npixels above our row */ +
		// 	  3*2*iBInRowOfUs/* 3*npixels before the 1st pixel of our 2x2 block  */
		// 	  + 0 /* save Uval in the blue pixel */ ] =
		//   pBMbytes[ 3*width*irow +
		// 	    3*2*iBInRowOfUs
		// 	    + 3 /* first byte of the 2nd pixel*/
		// 	    + 0 /* save Uval in the blue pixel */ ] = (uint8_t) Uval;


	      pRow[3*2*iBInRowOfUs /* 3*npixels before the 1st pixel of our 2x2 block */
		   + 0 /* save Uval in the blue pixel byte */ ] =
		pRow[3*2*iBInRowOfUs + 3 + 0] = (uint8_t) Uval;

		
		//go on to the next Uval
	      }
	    //Finished that one last row.
	    irow += 1;
	    nPixRowsToGo -= 1;
	  }
      }
    assert(fgetcCount == width*height + width*height/4);
  return;
}

//Here are supports for V value processing in which each YUV for each
// pixel is known and the corresponding GRB value is retrieved from
// the table and finally stored in the bmp, overwriting the saved Y and Us there.

// We'll use class bgrtriple from our bmp.h

//for FORMULA, not TABLE, version
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

static inline bgrtriple tripleFromYUVByFla( uint8_t y, uint8_t u, uint8_t v )
{
  return bgrtriple( uint8clamp (by*y + bu*u + bv*v +b1), //blue
	      uint8clamp (gy*y + gu*u + gv*v +g1), //green
	      uint8clamp (gy*y + gu*u + gv*v +g1) //green
			     );
}


//This may have regressed.
static inline void useVsFinishBmpByFla( int width, int height,
				   const uint8_t *pVs, uint8_t *pBMbytes)
{
  width = abs(width); height = abs(height);
  int halfwidth = width/2;

  uint8_t commonU; //For our usually 4 pixels
  uint8_t commonV; //For our usually 4 pixels
  
  bgrtriple T(0,0,0);
  
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
               /* We stored the correponding U values in the BGR array so that we
                  might process the Y, then U, then V values in a pipeline.  */
  int row = 0; /* pixel row we will process next, numbered from 0, also number of  rows finished.*/
  while( nPixRowsToGo > 0 )                                              
    {                                                                    
      if( nPixRowsToGo != 1 ) {                                          
	for( int x = 0; x < width; x += 2 ) {
	  /* 4 saves: */                                                 
	  { //first of an adjacent pair of rows
	    int iByte0Pix = 3*width*row + 3*x;
	   // common{U,V} will be used in 2nd code arg.
	   commonU = pBMbytes[ iByte0Pix ];
	   commonV = pVs[iUV];

	   T = tripleFromYUVByFla( pBMbytes[iByte0Pix + 1], commonU, commonV); 
	   pBMbytes[ iByte0Pix + 0 ] = T.tb;
	   pBMbytes[ iByte0Pix + 1 ] = T.tg;
	   pBMbytes[ iByte0Pix + 2 ] = T.tr;

	   T = tripleFromYUVByFla( pBMbytes[iByte0Pix + 3 + 1], commonU, commonV); 
	   pBMbytes[ iByte0Pix + 3 + 0 ] = T.tb;
	   pBMbytes[ iByte0Pix + 3 + 1 ] = T.tg;
	   pBMbytes[ iByte0Pix + 3 + 2 ] = T.tr;
	 }
	  { //second of the adjacent pair of rows
	    int iByte0Pix = 3*width*(row+1) + 3*x;
	    T = tripleFromYUVByFla( pBMbytes[iByte0Pix  + 1], commonU, commonV); 
	    pBMbytes[ iByte0Pix + 0 ] = T.tb;
	    pBMbytes[ iByte0Pix + 1 ] = T.tg;
	    pBMbytes[ iByte0Pix + 2 ] = T.tr;

	    T = tripleFromYUVByFla( pBMbytes[iByte0Pix + 3 + 1], commonU, commonV); 
	    pBMbytes[ iByte0Pix + 3 + 0 ] = T.tb;
	    pBMbytes[ iByte0Pix + 3 + 1 ] = T.tg;
	    pBMbytes[ iByte0Pix + 3 + 2 ] = T.tr;
	 }
	iUV++;
	}
	row += 2;
	nPixRowsToGo -= 2;                                               
      }                                                                  
      else {
	for( int x = 0; x < width; x += 2 ) {                     
	  /* 2 saves: */                                                 
	  { int iByte0Pix = 3*width*row + 3*x;
	   // common{U,V} will be used in 2nd code arg.
	   commonU = pBMbytes[ iByte0Pix ]; 
	   commonV = pVs[iUV];

	   T = tripleFromYUVByFla( pBMbytes[iByte0Pix + 1], commonU, commonV); 
	   pBMbytes[ iByte0Pix + 0 ] = T.tb;
	   pBMbytes[ iByte0Pix + 1 ] = T.tg;
	   pBMbytes[ iByte0Pix + 2 ] = T.tr;

	   T = tripleFromYUVByFla( pBMbytes[iByte0Pix + 3 + 1], commonU, commonV); 
	   pBMbytes[ iByte0Pix + 3 + 0 ] = T.tb;
	   pBMbytes[ iByte0Pix + 3 + 1 ] = T.tg;
	   pBMbytes[ iByte0Pix + 3 + 2 ] = T.tr;
	  }
	  iUV++;
	}
	row += 1;
	nPixRowsToGo -= 1;
      }
    }
  return ;
}

//back to TABLE version.
//
// Soon, there'll be a version where the table is included by as (assembler's) .incbin
// feature and linked with us as the assembled .o file.  We will refer to it
// through (I hope)
// extern uint8_t *inMemReferenceBMPFile
// uint8_t *inMemReferenceBGRs = inMemReferenceBMPFile + 54
// Maybe we'll program an application that writes out I420pRef4096x4096Frame.bmp
// (We have a separate program that constructs I420Ref4096x4096Frame.yuv)
 
//globals for YUV->BGR table.  Private for OneYuv2OneBmp.
//see referenceBMP.rst for documentation.

static uint8_t inMemReferenceBMPFile[54 + 3*4096*4096];
static uint8_t *inMemReferenceBGRs    = 0; //==0 when not we don't have it.
static const char *RefYUVFileName = "I420pRef4096x4096Frame.yuv";

bgrtriple bgrtripleFromYUVByTable( uint8_t Y, uint8_t U, uint8_t V)
{
  //We must avoid overflow in the index calc; don't do it in 8 bit arith!
  unsigned int iY = (unsigned int) Y;
  unsigned int iU = (unsigned int) U;
  unsigned int iV = (unsigned int) V;
  //signed int iI = (
  //  (3*4096/*bytes per row*/)*(4095-(16/*Yhs per V*/*iV + (iY>>4)/*yh*/)) +  /*Row of GRB byte array*/
  //  (3/*bytes per column*/  )*(16/*Yhs per U*/*iU + (iY&0xFF)/*yl*/)    /*Col of GRB byte array*/
  //		     );
  unsigned int iI = 3 * (
		   (4096)*( 4095 - ((16*iV) + (iY>>4)) ) + 16*iU + (iY & 0xFF)
			    );
  //uint8_t *pbgr = inMemReferenceBGRs+(
  //  (3*4096/*bytes per row*/)*(4095-(16/*Yhs per V*/*V + (Y>>4)/*yh*/)) +  /*Row of GRB byte array*/
  //  (3/*bytes per column*/  )*(16/*Yhs per U*/*U + (Y&0xFF)/*yl*/)    /*Col of GRB byte array*/
  //				      ); /* compute mem addr */
  uint8_t *pbgr = inMemReferenceBGRs + iI;
  return bgrtriple( pbgr[0] /* blue   */,
		    pbgr[1] /* green  */,
		    pbgr[2] /* red    */); /* retrieve from mem table */
  /* C/C++ lets us return whole data structures, not just builtins or pointers.*/
}

/* We created the YUV Reference file to represent an image so that reading it from
   top to bottom gives us pixels with increasing Y and V values.  We got that image
   represented as a Microsoft Classical (54-byte headers) from ffmpeg.  In I420p YUV files
   increasing offsets represent pixels from top to bottom, and left to right.  ffmpeg
   duly made a .bmp that looks like that.  However, the more offset pixel rows correspond
   to image positions more upward.  Therefore, we must reverse the row indexing with
   the (4095 - ...) code above.
*/

uint8_t *MemReferenceBGRTable()
{
  if( inMemReferenceBGRs ) return inMemReferenceBGRs;
  // We intend our yuv->bmp conversion function to be
  // called many times by one process--
  // Possibly by multiple threads in the future.
  
  //system( "ffmpeg -hide_banner -y -an -video_size 4096x4096 -i I420pRef4096x4096Frame.yuv  -frames:v 1 I420pRef4096x4096Frame.bmp" );

  // Ugly, inflexible, possibly inconsistent hack of a lazy programmer above.
  // but there is no need to make these parameters variable; except perhaps
  // ffmpeg may have color model parameters (alpha correction?) we might
  // take advantage of.
  //
  // yuv->bmp conversion is still mysterious; prelim experiment showed
  // 1 non-reversability.
  FILE* bmpfp;
  if( (!(bmpfp =  fopen("I420pRef4096x4096Frame.bmp", "r")))
      ||
      (1 != fread(inMemReferenceBMPFile,
		  54 + 3*4096*4096 /*sizeof(inMemReferenceBMPFile)*/, 1, bmpfp) ) )
    //Didn't help!
    {
      myerror("Missing or problem with I420pRef4096x4096Frame.bmp file.\n"
	      "See and use our Makefile to correct this problem.");
    }
  inMemReferenceBGRs = inMemReferenceBMPFile + 54;  //I know header length!
  // see if it looks good
  // for( int i = 0; i < 500; i++ )  printf("%d ", inMemReferenceBGRs[i]);
  
  return inMemReferenceBGRs;
}
  
static inline void useVsFinishBmpByTable( int width, int height,
				   const uint8_t *pVs, uint8_t *pBMbytes)
{
  width = abs(width); height = abs(height);
  int halfwidth = width/2;

  uint8_t commonU; //For our usually 4 pixels
  uint8_t commonV; //For our usually 4 pixels
  
  bgrtriple T(0,0,0);
  
  int nPixRowsToGo = height; /*Count down.  We go down two at a time */ 
  /*  until we get to 0 and are done or 1 when we do the one last row.*/ 
  int iUV = 0; /* index into the V (for us here) uint8_t array in the YUV file*/
               /* We stored the correponding U values in the BGR array so that we
                  might process the Y, then U, then V values in a pipeline.  */
  int row = 0; /* pixel row we will process next, numbered from 0, also number of  rows finished.*/
  while( nPixRowsToGo > 0 )                                              
    {                                                                    
      if( nPixRowsToGo != 1 )
	{                                          
	  for( int x = 0; x < width; x += 2 )
	    {
	      /* 4 saves: */                                                 
	      { //first of an adjacent pair of rows
		int iByte0Pix = 3*width*row + 3*x;
		// common{U,V} will be used in 2nd code arg.
		commonU = pBMbytes[ iByte0Pix ];
		commonV = pVs[iUV];
		
		T = bgrtripleFromYUVByTable( pBMbytes[iByte0Pix + 1], commonU, commonV); 
		pBMbytes[ iByte0Pix + 0 ] = T.tb;
		pBMbytes[ iByte0Pix + 1 ] = T.tg;
		pBMbytes[ iByte0Pix + 2 ] = T.tr;
		
		T = bgrtripleFromYUVByTable( pBMbytes[iByte0Pix + 3 + 1], commonU, commonV); 
		pBMbytes[ iByte0Pix + 3 + 0 ] = T.tb;
		pBMbytes[ iByte0Pix + 3 + 1 ] = T.tg;
		pBMbytes[ iByte0Pix + 3 + 2 ] = T.tr;
	      }
	      { //second of the adjacent pair of rows
		int iByte0Pix = 3*width*(row+1) + 3*x;
		T = bgrtripleFromYUVByTable( pBMbytes[iByte0Pix  + 1], commonU, commonV); 
		pBMbytes[ iByte0Pix + 0 ] = T.tb;
		pBMbytes[ iByte0Pix + 1 ] = T.tg;
		pBMbytes[ iByte0Pix + 2 ] = T.tr;

		T = bgrtripleFromYUVByTable( pBMbytes[iByte0Pix + 3 + 1], commonU, commonV); 
		pBMbytes[ iByte0Pix + 3 + 0 ] = T.tb;
		pBMbytes[ iByte0Pix + 3 + 1 ] = T.tg;
		pBMbytes[ iByte0Pix + 3 + 2 ] = T.tr;
	      }
	      iUV++;
	    }
	  row += 2;
	  nPixRowsToGo -= 2;                                               
	}                                                                  
      else //Last and odd row--may never happen.
	{
	for( int x = 0; x < width; x += 2 ) {                     
	  /* 2 saves: */                                                 
	  { int iByte0Pix = 3*width*row + 3*x;
	   // common{U,V} will be used in 2nd code arg.
	   commonU = pBMbytes[ iByte0Pix ]; 
	   commonV = pVs[iUV];

	   T = bgrtripleFromYUVByTable( pBMbytes[iByte0Pix + 1], commonU, commonV); 
	   pBMbytes[ iByte0Pix + 0 ] = T.tb;
	   pBMbytes[ iByte0Pix + 1 ] = T.tg;
	   pBMbytes[ iByte0Pix + 2 ] = T.tr;

	   T = bgrtripleFromYUVByTable( pBMbytes[iByte0Pix + 3 + 1], commonU, commonV); 
	   pBMbytes[ iByte0Pix + 3 + 0 ] = T.tb;
	   pBMbytes[ iByte0Pix + 3 + 1 ] = T.tg;
	   pBMbytes[ iByte0Pix + 3 + 2 ] = T.tr;
	  }
	  iUV++;
	}
	row += 1;
	nPixRowsToGo -= 1;
      }
    }
  return ;
}

static inline void useStreamVsFinishBmpByTable( int width, int height,
						FILE *YUVinFILE, uint8_t *pBMbytes, BMclass *pBM)
{
  width = abs(width); height = abs(height);
  int halfwidth = width/2;

  uint8_t commonU; //For our usually 2x2 pixel block
  uint8_t commonV; //For our usually 2x2 pixel block
  
  bgrtriple T(0,0,0);
  
  int nPixRowsToGo = height; /*Count down.  We go down two at a time */ 
  /*  until we get to 0 and are done or 1 when we do the one last row.*/
  
  int iUV = 0; /* index into the V (for us here) uint8_t array in the YUV file*/
               /* We stored the correponding U values in the BGR array so that we
                  might process the Y, then U, then V values in a pipeline.  */
  
  int irow = 0; /* row number of pixel row to process = num rows finished.*/
  
  // while( nPixRowsToGo > 0 )                                              
  //   { //Process one adjacent pair of rows:                                   
  //     if( nPixRowsToGo != 1 ) //check for last row when #rows is odd, which may never happen.
  // 	{ //Process the next pair of rows.                                         
  // 	  for( int x = 0; x < width; x += 2 ) //loop for the pairs of adjacent pixel columns.
  // 	    {
  // 	      /* 4 saves: */                                                 
  // 	      //first of an adjacent pair of rows
  // 	      int iByte0Pix = 3*width*irow + 3*x;
  // 	      // common{U,V} will be used in 2nd code arg.
  // 	      commonU = pBMbytes[ iByte0Pix + 0 ];
  // 	      //retrieve what was temporarilly saved in the
  // 	      // blue byte of the upper left pixel of the block.
	      
  // 	      { //get our 2x2 pixel block's commonV value from yuv stream.
  // 		assert( fgetcCount == width*height + width*height/4 + iUV );
  // 		int ret = FGETC(YUVinFILE);
  // 		if(ret == EOF) {
  // 		  myerror("storeStreamYsInBmp stream read EOF??");
  // 		}
  // 		commonV = (uint8_t) ret;
  // 	      }
	   
  // 		T = bgrtripleFromYUVByTable( pBMbytes[iByte0Pix + 1] /*Y value from the green byte*/,
  // 					     commonU, commonV);
  // 		//upper left of 4x4 (wrt. anti-Microsoft top-to-bottom order concept)
  // 		pBMbytes[ iByte0Pix + 0 ] = T.tb; //store final bgr pixel byte values ...
  // 		pBMbytes[ iByte0Pix + 1 ] = T.tg;
  // 		pBMbytes[ iByte0Pix + 2 ] = T.tr;

  // 		//upper right of 4x4 (wrt. anti-Microsoft top-to-bottom order concept)
  // 		T = bgrtripleFromYUVByTable( pBMbytes[iByte0Pix + 3 + 1], commonU, commonV); 
  // 		pBMbytes[ iByte0Pix + 3 + 0 ] = T.tb;
  // 		pBMbytes[ iByte0Pix + 3 + 1 ] = T.tg;
  // 		pBMbytes[ iByte0Pix + 3 + 2 ] = T.tr;
	      
  // 	       //second of the adjacent pair of rows
  // 		iByte0Pix = 3*width*(irow+1) + 3*x;
  // 		T = bgrtripleFromYUVByTable( pBMbytes[iByte0Pix  + 1], commonU, commonV); 
  // 		pBMbytes[ iByte0Pix + 0 ] = T.tb;
  // 		pBMbytes[ iByte0Pix + 1 ] = T.tg;
  // 		pBMbytes[ iByte0Pix + 2 ] = T.tr;
		
  // 		T = bgrtripleFromYUVByTable( pBMbytes[iByte0Pix + 3 + 1], commonU, commonV); 
  // 		pBMbytes[ iByte0Pix + 3 + 0 ] = T.tb;
  // 		pBMbytes[ iByte0Pix + 3 + 1 ] = T.tg;
  // 		pBMbytes[ iByte0Pix + 3 + 2 ] = T.tr;
	      
  // 	      iUV++;	  
  // 	      // for loop does x += 2.
  // 	    } // for loop for columns finish
  // 	  irow += 2;
  // 	  nPixRowsToGo -= 2;
  // 	}                                                                  
  //     else //last and odd row.  We may never have one.
  // 	{
  // 	  for( int x = 0; x < width; x += 2 )
  // 	    {                     
  // 	      /* 2 saves: */                                                 
  // 	      { int iByte0Pix = 3*width*irow + 3*x;
  // 		// common{U,V} will be used in 2nd code arg.
  // 		commonU = pBMbytes[ iByte0Pix ]; 

  // 		{ int ret = FGETC(YUVinFILE);
  // 		  if(ret == EOF) {
  // 		    myerror("storeStreamYsInBmp stream read EOF??");
  // 		  }
  // 		  commonV = (uint8_t) ret;
  // 		}

  // 		T = bgrtripleFromYUVByTable( pBMbytes[iByte0Pix + 1], commonU, commonV); 
  // 		pBMbytes[ iByte0Pix + 0 ] = T.tb;
  // 		pBMbytes[ iByte0Pix + 1 ] = T.tg;
  // 		pBMbytes[ iByte0Pix + 2 ] = T.tr;

  // 		T = bgrtripleFromYUVByTable( pBMbytes[iByte0Pix + 3 + 1], commonU, commonV); 
  // 		pBMbytes[ iByte0Pix + 3 + 0 ] = T.tb;
  // 		pBMbytes[ iByte0Pix + 3 + 1 ] = T.tg;
  // 		pBMbytes[ iByte0Pix + 3 + 2 ] = T.tr;
  // 	      }
  // 	    }
  // 	  iUV++;
  // 	  irow += 1;
  // 	  nPixRowsToGo -= 1;
  // 	}
  //   }


    while( nPixRowsToGo > 0 )                                              
    { //Process the next pair of rows ore the last one odd row.
      uint8_t *pRow = pBM->pR(irow); //first of the pair
      if( nPixRowsToGo != 1 ) //check for last row when #rows is odd, which may never happen.
	{ //Process a pair of rows.
	  uint8_t *p2ndRow = pBM->pR(irow + 1);
	  for( int x = 0; x < width; x += 2 ) //loop for the pairs of adjacent pixel columns.
	    {

	      /* 4 saves: */                                                 
	      //first of our adjacent pair of rows
	      //int iByte0Pix = 3*width*irow + 3*x;
	      // common{U,V} will be used in 2nd code arg.
	      commonU = // pBMbytes[ iByte0Pix + 0 ];
		pRow[3*x + 0];
	      //retrieve what was temporarilly saved in the
	      // blue byte of the upper left pixel of the block.
	      
	      { //get our 2x2 pixel block's commonV value from yuv stream.
		assert( fgetcCount == width*height + width*height/4 + iUV );
		int ret = FGETC(YUVinFILE);
		if(ret == EOF) {
		  myerror("storeStreamYsInBmp stream read EOF??");
		}
		commonV = (uint8_t) ret;
	      }
	   
	      T = bgrtripleFromYUVByTable( pRow[3*x + 1] /*bytes[iByte0Pix + 1]*/
					                 /*Y value from the green byte*/,
					     commonU, commonV);
		//upper left of 4x4 (wrt. anti-Microsoft top-to-bottom order concept)
		pRow[ 3*x + 0 ] = T.tb; //store final bgr pixel byte values ...
		pRow[ 3*x + 1 ] = T.tg;
		pRow[ 3*x + 2 ] = T.tr;

		//upper right of 4x4 (wrt. anti-Microsoft top-to-bottom order concept)
		T = bgrtripleFromYUVByTable( pRow[3*x + 3 + 1], commonU, commonV); 
		pRow[ 3*x + 3 + 0 ] = T.tb;
		pRow[ 3*x + 3 + 1 ] = T.tg;
		pRow[ 3*x + 3 + 2 ] = T.tr;
	      
	       //second of the adjacent pair of rows
	       //iByte0Pix = 3*width*(irow+1) + 3*x;
		T = bgrtripleFromYUVByTable( p2ndRow[3*x  + 1], commonU, commonV); 
		p2ndRow[ 3*x + 0 ] = T.tb;
		p2ndRow[ 3*x + 1 ] = T.tg;
		p2ndRow[ 3*x + 2 ] = T.tr;
		
		T = bgrtripleFromYUVByTable( p2ndRow[3*x + 3 + 1], commonU, commonV); 
		p2ndRow[ 3*x + 3 + 0 ] = T.tb;
		p2ndRow[ 3*x + 3 + 1 ] = T.tg;
		p2ndRow[ 3*x + 3 + 2 ] = T.tr;
	      
	      iUV++;	  
	      // for loop does x += 2.
	    } // for loop for columns finish
	  irow += 2;
	  nPixRowsToGo -= 2;
	}                                                                  
      else //last and odd row.  We may never have one.
	{
	  for( int x = 0; x < width; x += 2 )
	    {                     
	      /* 2 saves: */                                                 
	      { // common{U,V} will be used in 2nd code arg.
		commonU = pRow[ 3*x + 0]; 

		{ int ret = FGETC(YUVinFILE);
		  if(ret == EOF) {
		    myerror("storeStreamYsInBmp stream read EOF??");
		  }
		  commonV = (uint8_t) ret;
		}

		T = bgrtripleFromYUVByTable( pRow[3*x + 1], commonU, commonV); 
		pRow[ 3*x + 0 ] = T.tb;
		pRow[ 3*x + 1 ] = T.tg;
		pRow[ 3*x + 2 ] = T.tr;

		T = bgrtripleFromYUVByTable( pRow[3*x + 3 + 1], commonU, commonV); 
		pRow[ 3*x + 3 + 0 ] = T.tb;
		pRow[ 3*x + 3 + 1 ] = T.tg;
		pRow[ 3*x + 3 + 2 ] = T.tr;
	      }
	    }
	  iUV++;
	  irow += 1;
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
      myerror("Open OneTemp.yuv failed.");
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
      myerror("Read of OneTemp.bmp failed.");
    }
  size_t readcount = bmpreadret ;
  while ( readcount < yuvlen*2+54 )
    {
      fprintf(stderr, "error, or just short Read of OneTemp.bmp, we did read %ld bytes.\n", readcount);
      if( bmpreadret < 0 )
	{
	  myerror("Read of OneTemp.bmp failed.");
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


int OneYuv2OneBmpByFormula(unsigned int width, unsigned int height,
		  const uint8_t *YUVin, uint8_t *BMdata)
  /* YUVin points to the YUV I420p data in memory.
   BMdata points to memory for the bitmap byte triples.
  */
{
//
//
// Let size=width*height.
  storeYsInBmp( (int) width, (int) height, YUVin, BMdata);
//  Temporarily store size Y values in the 2nd byte of each of size bgr triples.
//
  storeUsInBmp( (int) width, (int) height, YUVin+width*height, BMdata);
//    with full yuv in mem at YArray,         ^---= YArray + width*height
//  Temporarily store given size/4 U values in 1st byte of each of size bgr triples. 
//
  useVsFinishBmpByFla( (int) width, (int) height, YUVin+width*height+(width/2)*(height/2), BMdata);
//    and then also                          ^--== YArray + width*height+(1/4)*width*height 
//  Use the stored Ys and Us, and the last size/4 V values,
//  to compute the size bgr BMP bgr triples were are converting to.
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
                a UorVrow, index iUorVrow every other pixel row.
              The pixel array row index is 2^4*iV + iYh, so the
                UorV array row index is half-rounded-down 
                                         = (2^3*iV) + (iYh>>1)
                 We only need to do the store operation when iHh&0x1==0,
                 since iV is CONSTANT for each iYh loop index value.
                

      ---------------------------------------------------------------

      Columns for each row;  iV and iYh are constant over a row, 
                 and a row of pixel's length is 2^12, so the
        Y linear index is 
           (2^12)*(2^4*iV + iYh) + (the column index) we'll see below.
   
        The UorV linear index is 
           ((2^12)/2)*(2^4iV + iYh) + floor((the column index)/2),
           need used only when (the column index) is even. 

              for iU, all 2^8 Uvalues,
                  for iHl, all 2^4 low order compontents of Yvalues,
                      retrieve (from the right i? var) and store the Y, U and V value,
                          whose pixel column index is 2*4*iU + iHl; 
			  a total 2^8*2^4 such triples;
                          Result: the Y linear index=(2^12)*(2^4*iV + iYh) + 2*4*iU + iHl

                          The Y array indices are identical to pixel indices;
                      
                          use an UorVcol index, iUorVcol, 
                              one for every adjacent pair of pixel cols;
                            half of 2*4*iU + iHl rounded down is
                                      4*iU + iHl>>1, use only when iH1&0x1==0
                          so the linear UorV index is:
                              ((2^12)/2)*(2^4*iV + iYh) + 4*iU + iHl>>1
                             to be used only when iHl&0x1==0.
                            
The Y array indices follow the pixel array indices
       iY = iP = 2^8*2^4*irow + icol

The U or V array is indexed to evenly located 2x2 blocks of pixels
(so U,V length is (1/4)*nPixels.


***************************************************************/ 
 
  const size_t pcols = 2<<(12-1);  //4096, 0xnumber of pixel columns   
  const size_t rows  = 2<<(12-1);  //4096, number of pixel (and bmp) rows
  
  // Unlike in newer langs, globals must be declared outside outside fns.
  // extern const char RefYUVFileName[];

  size_t psize = pcols*rows;  //number of pixels

  fprintf(stderr, "number of pixels=0x%lx\n", psize);
  fprintf(stderr, "yuv file size=0x%lx\n",psize + psize/4 + psize/4);
  size_t yuvFileSize = psize + psize/4 + psize/4;
  uint8_t *Y = new uint8_t[yuvFileSize];
  if( Y == 0 )
    {
      error(1, errno, "Failed mem alloc for the %ld byte array.",
	    (yuvFileSize));
    }
  //byte array for Y followed by the rest of YUV data 

  uint8_t *U = Y + psize;   //U and V arrays are after the psize'd U array.
  uint8_t *V = U + psize/4; //V array is only psize/4.  It's the last in file.
  
  int pixRowParity = 0;
  int iUandV = 0;

  int iPix = 0;

  for( int iV = 0; iV < 256; iV++ )
    { /*
       */
      for( int iYh = 0; iYh < 16; iYh++ )
	{ /*
	   */
	  for( int iU = 0; iU < 256; iU++ )
	    { /*
	       */
	      for(int iYl = 0; iYl < 16; iYl++)
		{ /*

		   */
		  iPix++;
		  //if( iV < 12 && iU < 12)
		  //{
		  //  printf("iV=%d iYh=%d iU=%d iYl=%d "
		  //	     "Y[%d]=%d \n", iV, iYh, iU, iYl,
		  //	     (4096*(16*iV + (iYh)) + 16*iU + iYl),
		  //	     (iYh<<4) + iYl);
		  //}
		  /* C/C++ bit shift >> is VERY LOW PRECEDENCE!!
		     resulting in very long time to fix bugs. */
		  Y[4096*(16*iV + (iYh)) + 16*iU + iYl] = (iYh<<4) + iYl;
		  if( ((iYh & 0x1)==0) && ((iYl & 0x1)==0) )
		  {
		    size_t where = 1024*(16*iV + iYh)+ 8*iU + (((unsigned int) iYl)>>1);
		    // had been 2048
		    // had been 8, is this correct?----^ 
		    assert( where < psize/4 );
		    U[where] = iU;
		    V[where] = iV;
		    //assert( iU == 0 );
		    //assert( iV == 0 );
		  }
		 //end of iYl loop  
		}
	      // end of iU loop
	    }
	  // end of iU loop
	}
      //end of iV loop
    }
  assert(iPix == 4096*4096);

  FILE* fp = fopen(RefYUVFileName, "w");
  if(!fp) {
    error(1, errno, "Output file %s opening failed.", RefYUVFileName);
    assert(0);
  }
  size_t wret = fwrite( Y, yuvFileSize, 1, fp);
  if( wret != 1 )
    {
      error(1, errno, "Failed or short file %s writing.", RefYUVFileName);
      assert(0);
    }

  int cret = fclose(fp);
  if(cret) {
    error(1, errno, "Closing of %s failed.", RefYUVFileName);
    assert(0); 
  }

  return 0;

}

int OneYuv2OneBmpByTable(unsigned int width, unsigned int height,
		  const uint8_t *YUVin, uint8_t *BMdata)
  /* YUVin points to the YUV I420p data in memory.
   BMdata points to memory for the bitmap byte triples.
  */
{
  MemReferenceBGRTable();
//
//
// Let size=width*height.
  storeYsInBmp( (int) width, (int) height, YUVin, BMdata);
//  Temporarily store size Y values in the 2nd byte of each of size bgr triples.
//
  storeUsInBmp( (int) width, (int) height, YUVin+width*height, BMdata);
//    with full yuv in mem at YArray,         ^---= YArray + width*height
//  Temporarily store given size/4 U values in 1st byte of each of size bgr triples. 
//
  useVsFinishBmpByTable( (int) width, (int) height, YUVin+width*height+(width/2)*(height/2), BMdata);
//    and then also                          ^--== YArray + width*height+(1/4)*width*height 
//  Use the stored Ys and Us, and the last size/4 V values,
//  to compute the size bgr BMP bgr triples were are converting to.
//
// This sequence reads the bytes of the I420p YUV raw image SEQUENTIALLY,
// supporting a pipeline to process a sequence of raw image frames.
//
//
  return 0;
}


int OneYuvStream2OneBmpByTable(unsigned int width, unsigned int height,
			       FILE *YUVinFILE, uint8_t *BMdata, BMclass *pBM)
  /* *YUVinFILE is stream of YUV I420p images.
   BMdata points to memory for the bitmap byte triples.
  */
{
  MemReferenceBGRTable();
//
  fgetcCount = 0;
  /* 
     int c; int cnt = 0;
     fprintf(stderr, "%p \n", YUVinFILE);
     while( EOF != (c = fgetc(YUVinFILE) ))
     { fprintf(stderr, "%d %d \n", cnt++, c);
     }
  */

//
// Let size=width*height.
  storeStreamYsInBmp( (int) width, (int) height, YUVinFILE, BMdata, pBM);
//  Temporarily store size Y values in the 2nd byte of each of size bgr triples.
//
  storeStreamUsInBmp( (int) width, (int) height, YUVinFILE, BMdata, pBM);
//  Temporarily store given size/4 U values in 1st byte of each of size bgr triples. 
//
  useStreamVsFinishBmpByTable( (int) width, (int) height, YUVinFILE, BMdata, pBM);
//  Use the stored Ys and Us, and the last size/4 V values,
//  to compute the size bgr BMP bgr triples were are converting to.
//
// This sequence reads the bytes of the I420p YUV raw image SEQUENTIALLY,
// supporting a pipeline to process a sequence of raw image frames.
//
//
  return 0;
}
