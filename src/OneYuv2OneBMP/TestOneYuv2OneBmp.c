#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "OneYuv2OneBmp.h"
#include <error.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

char *useage = "TestOneYuv2OneBmp wwwwxhhhh infile.yuv outfile.bmp";

int main(int argc, char **argv)
{
  unsigned int width;
  unsigned int height;
  char *YUVinFilename;
  char *BMPinFilename;
  char xTent;
  
  if( argc != 4 )
    { error(1, 0, "missing args\n%s", useage); }

  if( 3 != sscanf( argv[1], "%u%c%u", &width, &xTent, &height )||
      xTent != 'x' )
    { error(1, 0, "Bad <width>x<height> arg. %s", useage); }

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
  char *pyuv = malloc(yuvsize);
  if (!pyuv) { error(1, errno, "Malloc for yuv failed."); }
  
  int yuvFD = open(YUVinFilename, O_RDONLY, 0);
  if( yuvFD < 0 ) {
    error(1, errno, "Failed to open %s", YUVinFilename) ;
  }

  ssize_t nread = read( yuvFD, pyuv, yuvsize);
  if( nread != yuvsize ) {
    error(1, errno, "yuv read ret %ld, didn't read %d bytes.", nread, yuvsize);
  }

  char *pbmp = malloc(rgblen + 54);
  if( !pbmp ) {error(1, errno, "Malloc for bmp failed."); }

  int funret = OneYuv2OneBmp( width, height, pyuv, pbmp );
  //printf("OneYuv2OneBmp ret %d \n", funret);

  int bmpFD = open(BMPinFilename, O_CREAT | O_RDWR, S_IRUSR|S_IWUSR);
  if( bmpFD < 0 ) {
    error(1, errno, "Failed to open result file %s\n", BMPinFilename);
  }
  ssize_t writeret = write(bmpFD, pbmp, 2*yuvsize+54);
  
  return 0;
  
}
