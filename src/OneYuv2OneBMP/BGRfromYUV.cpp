#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <error.h>
#include <stdint.h>

int main(int argc, char *argv[])
{
  uint8_t *pbm = new uint8_t[4096*4096*3 + 54];
  if( !pbm ) {
    error(1, errno, "Can't alloc memory for the Universal YUV bmp.");
  }
  FILE* bmpfp;
  if( (!(bmpfp =  fopen("I420pRef4096x4096Frame.bmp", "r")))
      ||
      (1 != fread(pbm,4096*4096*3 + 54 , 1, bmpfp) ) )
    {
      error(1, errno, "Missing or problem with I420pRef4096x4096Frame.bmp file.\n"
	      "See and use our Makefile to correct this problem.");
    }
  pbm = pbm+54;
  printf("We use decimal.\n");

  char *line = NULL;
  size_t len = 0;
  ssize_t nread;

  
  while(-1 != (printf("Y U V> "), nread = getline(&line, &len, stdin)))
    {
      unsigned int Y;
      unsigned int U;
      unsigned int V;

      if( 3 != sscanf(line,"%u %u %u", &Y, &U, &V))
	{
	  printf("didn't read 3 integers\n");
	  continue;
	}
      if ( !(0 <= Y) || !(Y <= 255) ||
	   !(0 <= U) || !(U <= 255) ||
	   !(0 <= V) || !(V <= 255) )
	{
	  printf("Only enter YUVs in the [0, 255] range.\n");
	  continue;    
	}

      int i = 3 * (
		   (4096)*( 4095 - ((16*V) + (Y>>4)) ) + 16*U + (Y&0xFF)
			    );
      printf("B=%u G=%u R=%u\n\n", pbm[i], pbm[i+1], pbm[i+2]);
    }
  printf("\n");
  return 0;
}
	       
      
