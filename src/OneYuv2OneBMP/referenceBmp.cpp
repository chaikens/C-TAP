#include "bmp.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <error.h>
#include <assert.h>

int main(int argc, char **argv) {

   int squareSize = 2;
   uint8_t valSeq[8];
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
   
   //8*8*8 = 2^9 = 512
   // = (8*4)*(8*2)
   // so, img will have
   // (8*4)=32 columns of color squares
   //          squareSize=2 => 32*2 = 64 pixels width
   // (8*2)=16 rows                 = 32 pixels height
   //
   // 1st blue value
   //   8 rows, one for each green value
   //    1st green value
   //      (8vals)(8vals)(8vals)(8vals)(8vals)(8vals)(8vals)(8vals)
   //       reds   reds   reds   reds   reds   reds  reds   reds
   //
   //    2nd and remaining green values for total of 8
   //      (8vals)(8vals)(8vals)(8vals)(8vals)(8vals)(8vals)(8vals)
   //        reds   reds   reds   reds   reds   reds  reds   reds
   //
   //    ...
   //
   // 2nd and remaining blue values for a total of 8
   //    8 rows, one for each green value
   //
   //    .....
   //      .....
   //
   //
   //   2 rows for each of 8 blue values
   //  so we have (8*2)=16 rows
   //         squareSize=2 => 16*2 = 32 pixel height

   FULLBITMAP *pbm = makeClassicBitmap(64,32);

   int nsquares = 0;

   int irow = 0;
   int icol = 0;

   for (int bluei = 0; bluei < 8; bluei++)
     { // generate 8 pairs of rows of squares
       // In each pair, the first row=bluei, second=bluei+1
       uint8_t bval = valSeq[bluei];
       printf("%02x\n", bval);
       for(int greeni = 0; greeni < 8; greeni++)
	 { //  generate one pair of rows
	   uint8_t gval = valSeq[greeni];
	   printf("   %02x\n", gval);
	   for(int redi = 0; redi < 8; redi++)
	     {
	       /*
		 bool drawRect( uint16_t xmin, uint16_t rwidth,
		 uint16_t ymin, uint16_t rheight,
		 uint8_t r, uint8_t g, uint8_t b );
	       */
	       printf("(ix=%d,iy=%d), %02x%02x%02x\n", icol, irow, bval, gval, valSeq[redi]);
	       //int ret = pbm->drawRect( squareSize*(icol), squareSize, squareSize*(irow), squareSize,
	       //				valSeq[redi], gval, bval);
	       //assert( ret == true );
	       nsquares++;
	       icol += 1;
	     }
	   icol = 0;
	   irow += 1; //green loop
	 }
       //blue loop
     }

   printf("\nnumber of squares written=%d\n", nsquares);

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

