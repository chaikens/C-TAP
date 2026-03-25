#include <unistd.h>
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

const char *usage = "TestOneYuv2OneBmp [--fast] wwwwxhhhh infile.yuv outfile.bmp";

int main(int argc, char **argv)
{
  unsigned int width;
  unsigned int height;
  char *YUVinFilename;
  char *BMPinFilename;
  char xTent;
  
  if( argc < 4 )
    { error(1, 0, "missing args\n%s", usage); }

  bool fast;
  
  if( argc == 5 ) {fast = true; argv++;}
  else {fast = false;}
  
  if( 3 != sscanf( argv[1], "%u%c%u", &width, &xTent, &height )||
      xTent != 'x' )
    { error(1, 0, "Bad <width>x<height> arg. %s", usage); }

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
  
  if(fast)
    {
      
      funret = OneYuv2BmpDataFast( width, height, pyuv, pbmp+54 );
    }
  else
    {
      funret =     OneYuv2OneBmp( width, height, pyuv, pbmp );
      //this old function will remake the header, that doesn't hurt.
      //printf("OneYuv2OneBmp ret %d \n", funret);
    }
  int bmpFD = open(BMPinFilename, O_CREAT | O_RDWR, S_IRUSR|S_IWUSR);
  if( bmpFD < 0 ) {
    error(1, errno, "Failed to open result file %s\n", BMPinFilename);
  }
  ssize_t writeret = write(bmpFD, pbmp, 2*yuvsize+54);
  
  return 0;
  
}
