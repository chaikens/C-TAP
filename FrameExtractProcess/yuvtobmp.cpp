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

#define myerror(message) {fprintf(stderr, "OneYuv2OneBmp.cpp error: %s \n", message); assert(0); }


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
	   int Yval = fgetc(YUVinFILE);
	   if(Yval == EOF) {
	     myerror("storeStreamYsInBmp stream read EOF??");
	   }
	   prow[3*iPixcol + 1] = (uint8_t) Yval;
	   //Store temporarilly in the green byte.
	}
    }
    //assert(fgetcCount == width*height);
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
  //assert((width == 2*halfwidth) );

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

static inline void useStreamVsFinishBmpByFunPointer( FunPointerType FunPointer, int width, int height,
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
	   
	      T = FunPointer( pRow[3*x + 1] /*bytes[iByte0Pix + 1]*/
					                 /*Y value from the green byte*/,
					     commonU, commonV);
		//upper left of 4x4 (wrt. anti-Microsoft top-to-bottom order concept)
		pRow[ 3*x + 0 ] = T.tb; //store final bgr pixel byte values ...
		pRow[ 3*x + 1 ] = T.tg;
		pRow[ 3*x + 2 ] = T.tr;

		//upper right of 4x4 (wrt. anti-Microsoft top-to-bottom order concept)
		T = FunPointer( pRow[3*x + 3 + 1], commonU, commonV); 
		pRow[ 3*x + 3 + 0 ] = T.tb;
		pRow[ 3*x + 3 + 1 ] = T.tg;
		pRow[ 3*x + 3 + 2 ] = T.tr;
	      
	       //second of the adjacent pair of rows
	       //iByte0Pix = 3*width*(irow+1) + 3*x;
		T = FunPointer( p2ndRow[3*x  + 1], commonU, commonV); 
		p2ndRow[ 3*x + 0 ] = T.tb;
		p2ndRow[ 3*x + 1 ] = T.tg;
		p2ndRow[ 3*x + 2 ] = T.tr;
		
		T = FunPointer( p2ndRow[3*x + 3 + 1], commonU, commonV); 
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

		T = FunPointer( pRow[3*x + 1], commonU, commonV); 
		pRow[ 3*x + 0 ] = T.tb;
		pRow[ 3*x + 1 ] = T.tg;
		pRow[ 3*x + 2 ] = T.tr;

		T = FunPointer( pRow[3*x + 3 + 1], commonU, commonV); 
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

static inline void useStreamVsFinishBmpByTable( int width, int height,
						FILE *YUVinFILE, uint8_t *pBMbytes, BMclass *pBM)
{
  useStreamVsFinishBmpByFunPointer( bgrtripleFromYUVByTable,
				    width, height,
				    YUVinFILE, pBMbytes, pBM);
}






int yuvtobmpTcom(unsigned int width, unsigned int height,
			       FILE *YUVinFILE, uint8_t *BMdata, BMclass *pBM)
  /* *YUVinFILE is stream of YUV I420p images.
   BMdata points to memory for the bitmap byte triples.
  */
{
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



