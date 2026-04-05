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
   FULLBITMAP *pbm = makeClassicBitmap(32,24);
   printf("\ndrawRect RETURN=%d\n", pbm->drawRect(1,4,3,5,32,64,96));

   const char *BMPFilename = "testbmp.bmp";
   int bmpFD = open(BMPFilename, O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR|S_IWUSR);
  if( bmpFD < 0 ) {
    error(1, errno, "Failed to open result file %s\n", BMPFilename);
  }

  ssize_t writeret = write(bmpFD, pbm, pbm->headers.fileHeader.FileSize);
  assert( writeret == pbm->headers.fileHeader.FileSize );
  close(bmpFD);
  

   return 0;
 }
