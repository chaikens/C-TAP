#include "OneYuv2OneBmp.h"
#include "bmp.h"

#include <stdio.h>
#include <error.h>
#include <errno.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  size_t width = 0;
  size_t height  = 0;
  int filedescriptor = -1;
  int c; //for options
  while(1) {
    static struct option long_options[] = {
      {"fd", required_argument, 0, 0 },
      { 0, 0, 0, 0 }
    };
      c = getopt_long(argc, argv, "", long_options, 0);
      if( c == -1 ) break;
    filedescriptor = atoi(optarg);
  }

  if( (1 != sscanf(argv[argc-2], "%lu", &width) ) ||
      (1 != sscanf(argv[argc-1], "%lu", &height)))
    {
      error(1, 0, "YUVToBMPStreamFilter <width> <height> [--fd <Unix file descr>] \n: (stdin, stream of YUV I420p) --> (stdout, stream of classical bmp)");
    }

  const size_t BMPsize = 54 + 3*width*height;  //reuse for each image
  uint8_t *pBMP = (uint8_t *) makeClassicBitmap(width, height);
  //allocates memory and checks errors.
  //54 byte header will remain unchanged

  uint8_t  * const pBMdata = pBMP + 54;

  BMclass *pBM = new BMclass(width, height, pBMdata, pBMP);

  
  unsigned int imgCount = 0;

  FILE *inputFP = stdin;
  if( filedescriptor > 0 )
    {
      if(!(inputFP = fdopen(filedescriptor, "r")))
	{
	  error(1, errno, "fdopen on %d failed", filedescriptor);
	}
    }
      

  int firstc; //Apparently, Unix doesn't make feof(..) != 0 until
              //after we try to read!
  while( (firstc = fgetc(inputFP)) != EOF )
    { 
      ungetc(firstc, inputFP);

      OneYuvStream2OneBmpByTable(width, height, inputFP, pBMdata, pBM);

      size_t ret = fwrite(pBMP, BMPsize, 1, stdout);
      if( ret != 1 ) {
	error(1, errno, "YUVToBMPStreamFilter write bmp to stdout failed??");
      }
      imgCount++;
    }

  fprintf(stderr, "YUVToBMPStreamFilter transformed %u images.\n", imgCount);
  fclose(inputFP);
  fclose(stdout);

  return 0;
  
}

  
      
