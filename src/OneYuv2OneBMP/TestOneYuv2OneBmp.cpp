#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include "OneYuv2OneBmp.h"
/*
 * gives us uint8_t uint16_t for our image and video files
 */
#include <error.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

const char *usage = "TestOneYuv2OneBmp [--table|--formula] wwwwxhhhh infile.yuv outfile.bmp"
  "\nDefault is --table";

int main(int argc, char **argv)
{
  //options processing first
  int useformula = 0; /* default is to use the table */
  int digit_optind = 0;
  static struct option long_options[] = {
    {"table",   no_argument, &useformula, 0 },
    {"formula", no_argument, &useformula, 1 },
    {0, 0, 0, 0}
  };

  while( -1 != getopt_long( argc, argv,	 /* optstring */ "",
			    long_options,
			    /* &longindex */ 0 ) )
    ;
  
  
  unsigned int width;
  unsigned int height;
  char *YUVinFilename;
  char *BMPinFilename;
  char xTent;
  
  if( 3 != sscanf( argv[1], "%u%c%u", &width, &xTent, &height )||
      xTent != 'x' )
    { error(1, 0, "Bad <width>x<height> arg. %s", usage); }

  if( !argv[2] || argv[2][0]=='-' ) {
    error(1, 0, "Bad infile.yuv filename.");
  }

  if( !argv[3] || argv[3][0]=='-' ) {
       error(1, 0, "Bad outfile.bmp filename.");
  } 

  YUVinFilename = argv[2];
  BMPinFilename = argv[3];
  
  unsigned int npixels = width * height;
  unsigned int rgblen = npixels * 3;
  unsigned int uORvSize = npixels/4;
  unsigned int yuvsize = npixels + 2*(uORvSize);
  if( yuvsize != (npixels*3)/2 )
    {
      printf("yuvsize not 3/2*npixels??");
    }
  uint8_t *pyuv = (uint8_t *) malloc(yuvsize);
  if (!pyuv) { error(1, errno, "Malloc for yuv failed."); }
  
  int yuvFD = open(YUVinFilename, O_RDONLY, 0);
  if( yuvFD < 0 ) {
    error(1, errno, "Failed to open %s", YUVinFilename) ;
  }

  ssize_t nread = read( yuvFD, pyuv, yuvsize);
  if( nread != yuvsize ) {
    error(1, errno, "yuv read ret %ld, didn't read %d bytes.", nread, yuvsize);
  }

  uint8_t *pbmp = (uint8_t *) makeClassicBitmap( width, height );
  // storage alloc checked by this


  int funret;
  
  if(useformula)
    {
      
      funret = OneYuv2OneBmpByFormula( width, height, pyuv, pbmp+54 );
      flipClassicBitmap((FULLBITMAP *) pbmp);
    }
  else
    {
      funret = OneYuv2OneBmpByTable( width, height, pyuv, pbmp );
      //this old function will remake the header, that doesn't hurt.
      //printf("OneYuv2OneBmp ret %d \n", funret);
    }
  int bmpFD = open(BMPinFilename, O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR|S_IWUSR);
  if( bmpFD < 0 ) {
    error(1, errno, "Failed to open result file %s\n", BMPinFilename);
  }
  ssize_t writeret = write(bmpFD, pbmp, 2*yuvsize+54);
  
  return 0;
  
}
