#include "bmp.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <error.h>
#include <assert.h>
#include <cstring>

int main(int argc, char **argv) {

  bool verbose = false;
  if(argc == 2 && ( strcmp(argv[1],"-v")==0 || strcmp(argv[1],"--verbose")==0)) {
    verbose = true;
  }

  const int nvalSeq = 8;
  uint8_t valSeq[nvalSeq];
   /*
   for (int v = 0; v <= 6; v++)
     valSeq[v] = 0x20 + v*0x20;
   valSeq[7] = 0xff;
   */
   valSeq[0] = 0;
   valSeq[1] = valSeq[0] + 36;
   valSeq[2] = valSeq[1] + 36;
   valSeq[3] = valSeq[2] + 36;
   valSeq[4] = valSeq[3] + 36;
   valSeq[5] = valSeq[4] + 37;
   valSeq[6] = valSeq[5] + 37;
   valSeq[7] = valSeq[6] + 37;
   assert(valSeq[7] == 255 );
   
   const int ncolors    = nvalSeq*nvalSeq*nvalSeq; //no int exp operator in C/C++
   //8*8*8 = 2^9 = 512
   // = (8*4)*(8*2)
   // so, img will have
   // (8*4)=32 columns of color squares
   
   const int nSqrsPerRow = 32; // which == number of columns
   const int nSqrsPerCol = ncolors/nSqrsPerRow;

   const int sqrSize = 2;
   const int bmpWidth = nSqrsPerRow*sqrSize;
   const int bmpHeight = nSqrsPerCol*sqrSize;
   
   //          squareSize=2 => 32*2 = 64 pixels width
   // (8*2)=16 rows                 = 32 pixels height
   //

   
   FULLBITMAP *pbm = makeClassicBitmap(bmpWidth,bmpHeight);

   int isquare = 0;

   int irow = 0;
   int icol = 0;

   // simply generate the grbs in desired order
   for (int bluei = 0; bluei < 8; bluei++)
     {
       uint8_t bval = valSeq[bluei];
       for(int greeni = 0; greeni < 8; greeni++)
	 {
	   uint8_t gval = valSeq[greeni];
	   for(int redi = 0; redi < 8; redi++)
	     {
	       uint8_t rval = valSeq[redi];
	       irow = isquare/nSqrsPerRow;
	       icol = isquare - irow*nSqrsPerRow;
	       if(verbose) printf("(ix=%d,iy=%d), %02x%02x%02x\n", icol, irow, bval, gval, valSeq[redi]);
	       /*
		 bool drawRect( uint16_t xmin, uint16_t rwidth,
		 uint16_t ymin, uint16_t rheight,
		 uint8_t r, uint8_t g, uint8_t b );
	       */

	       int ret = pbm->drawRect( sqrSize*(icol), sqrSize, sqrSize*(irow), sqrSize,
	       				rval, gval, bval);
	       assert( ret == true );
	       isquare++;
	     }
	 }
     }

   if(verbose) printf("\nnumber of squares written=%d\n", isquare);

   const char *BMPFilename = "referenceBmp.bmp";
   int bmpFD = open(BMPFilename, O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR|S_IWUSR);
  if( bmpFD < 0 ) {
    error(1, errno, "Failed to open result file %s\n", BMPFilename);
  }

  ssize_t writeret = write(bmpFD, pbm, pbm->headers.fileHeader.FileSize);
  assert( writeret == pbm->headers.fileHeader.FileSize );
  close(bmpFD);
  return 0;
 }

