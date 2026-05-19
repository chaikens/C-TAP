/**
 yuvSelectFilter WidthxHeight framenums-fd yuvinput-fd yuvoutput-fd
 -fd's are small integers.

 This program doesn't look inside yuv frames, but assumes their
 length is (3*width*height)/2, which is of I420p format.
*/

#include <stdio.h>
#include <error.h>
#include <errno.h>
#include <iostream>
#include <getopt.h>
#include <stdint.h> //for bytes=uint8_t
using namespace std;

static char usage[] = "yuvSelectFilter WidthxHeight framenums-fd yuvinput-fd yuvoutput-fd\n\
-fd's are small integers.\n";

static int vb = 0;

static int get_our_options( int *argc, char **argv[]);

uint8_t *yuvbuf = 0;

int main(int argc, char *argv[])
{
  get_our_options(&argc, &argv); //lops off specified --options, just --verbose for now.
  unsigned int width, height;
  int fnsfd, yuvinfd, yuvoutfd;
  int ret; //let's reuse, ugh.
  if( argc != 5 )
    {
      error(1, 0, usage);
    }
  if ( 2 != (ret = sscanf(argv[1],"%ux%u", &width, &height)))
    {
      error(1, 0, "First param %s should be WxH, both decimal strings\n%s", argv[1], usage);
    }
  if ( ((width & 1) != 0) || ((height & 1) != 0) )
    {
      error(1, 0, "width %d and height %d must be even (don't know what if not) for us."
	    "\n Though, %s doesn't look inside yuv frames",
	    width, height, argv[0]);
    }
  size_t yuvsize = (width*height*3)/2;
  yuvbuf = new uint8_t[yuvsize];
  if (! yuvbuf) {
    error(1, errno, "Cant allocate size %lu buffer.", yuvsize);
  }
  
  if( (1 != (ret = sscanf(argv[2], "%d", &fnsfd)))   ||
      (1 != (ret = sscanf(argv[3], "%d", &yuvinfd))) ||
      (1 != (ret = sscanf(argv[4], "%d", &yuvoutfd))))
    {
      error(1, 0, "Problem with params 1,2 or 3\n%s", usage);
    }
  //cerr << width << " " << height <<  " " <<  fnsfd <<  " " << yuvinfd << " " <<  yuvoutfd << endl;





  FILE *fnsFP = fdopen(fnsfd, "r");
  if ( ! fnsFP )
    {
      error(1,errno, "Cant open fd %d for reading as a FILE.", fnsfd);
    }
  
  FILE *yuvinFP = fdopen(yuvinfd, "r");
  if ( ! yuvinFP )
    {
      error(1,errno, "Cant open fd %d for reading as a FILE.", yuvinfd);
    }

  FILE *yuvoutFP = fdopen(yuvoutfd, "w");
  if ( ! yuvoutFP )
    {
      error(1,errno, "Cant open fd %d for writing as a FILE.", yuvoutfd);
    }

  int fcount = 0;
  int fwanted;
  int didreadframe = 0;
  while ( (didreadframe = fscanf(fnsFP, "%d", &fwanted), didreadframe == 1)  )
    {
      //Must read to somewhere even though we throw some away
      //Maybe we'll develop double buffering if performance is bad.

      size_t rret = fread( yuvbuf, yuvsize, 1, yuvinFP );
      if ( rret != 1 ) {
	break;  //out of reading wanted frame number loop
      }
      fcount++;
      if( fwanted == fcount ) {
	  //We love you!
	size_t wret = fwrite( yuvbuf, yuvsize, 1, yuvoutFP );
	if( wret != 1 ) {
	  error( 1, errno, "Failure to write frame %ul to output stream.", fwanted);
	    }
	}
      else {
	//We discard boring pictures.
      }
    }
  
  
  return 0;
}

static int get_our_options( int *argc, char **argv[])
{
  extern char *optarg; //globals from getopt and getopt_long
  extern int optind, opterr, optopt;

  int c;
  int digit_optind = 0;
  int num_args_gotten = 0; //for updating *argc and *argv
  //so original argument getting works

  //Logic here adapted from man 3 getopt
  while (1) {
    int this_option_optind = optind ? optind : 1;
    int option_index = 0;
    static struct option long_options[] = {
      {"verbose", no_argument, &vb, 1},  //0
      {0,         0,                 0,  0 }
    };
    c = getopt_long( *argc, *argv, "",
		    long_options, &option_index);
    if (c == -1)
      break;
    /*
    switch (c) {
    case 0:
      switch (option_index) { 
      case 0: break; 
      }
    }
    */
  }

  char *cmd = (*argv)[0];
  *argc = *argc - (optind - 1);
  *argv = *argv + (optind - 1);
  (*argv)[0] = cmd;
  return 0;
}
