/**
 yuvSelectFilter WidthxHeight framenums-fd yuvinput-fd [at least one option]
 -fd's are small integers.

There are 3 use cases specified by mutually exclusive options and suboptions:

--yuv-out-stream-fd <fd>

--bmp-out-stream-fd <fd> [--bmp-conversion kind(TBD)]

--bmp-out-dirpath <writable dir name for .bmp files> [--bmp-conversion kind(TBD)]

optional options:
--verbose

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

static char usage[] = "yuvSelectFilter WidthxHeight framenums-fd yuvinput-fd [one option required]\n\
-fd's are small integers.\n";

//options
static int vb = 0;
static int do_yuv_stream_out = 0;
static int yuvoutfd = -1;
static int do_bmp_stream_out = 0;
static int bmpoutfd = -1;
static int do_bmp_dir_out = 0;
static char *bmpdirpath = 0;
static const char *bmpnameprefix = "thumb";
static const char *bmpconversion = 0;

//FILE POINTERS
static FILE *fnsFP = 0;    //required, frame numbers wanted
static FILE *yuvinFP = 0;  //required, yuv frame input stream
static FILE *yuvoutFP = 0; //optional, yuv frame output stream
static FILE *bmpoutFP = 0;


static int get_our_options( int *argc, char **argv[]);

static void optionprocess() {
  int sum = do_yuv_stream_out + do_bmp_stream_out + do_bmp_dir_out;
  if ( sum != 1) {
    error(1, 0, "Exactly one output option must be given, not %d.", sum);
  }

  if( do_yuv_stream_out ) {
    yuvoutFP = fdopen(yuvoutfd, "w");
    if ( ! yuvoutFP )
      {
	error(1,errno, "Cant open fd %d for writing as a FILE.", yuvoutfd);
      }
  }

  if( do_bmp_stream_out ) {
    bmpoutFP = fdopen(bmpoutfd, "w");
    if ( ! bmpoutFP ) {
      {
	error(1, errno, "Cant open fd %d for writing as a FILE.", bmpoutfd);
      }
    }
  }
}

uint8_t *yuvbuf = 0;

 int main(int argc, char *argv[]) {
  get_our_options(&argc, &argv); //lops off specified --options, just --verbose for now.
  unsigned int width, height;
  int fnsfd, yuvinfd, yuvoutfd;
  int ret; //let's reuse, ugh.
  if( argc != 4 )
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

  optionprocess();
  

  fnsFP = fdopen(fnsfd, "r");
  if ( ! fnsFP )
    {
      error(1,errno, "Cant open fd %d for reading as a FILE.", fnsfd);
    }
  
  yuvinFP = fdopen(yuvinfd, "r");
  if ( ! yuvinFP )
    {
      error(1,errno, "Cant open fd %d for reading as a FILE.", yuvinfd);
    }
  
  int fcount = 0;
  int fwanted;
  int didreadframe = 0;
  //Loop to read the next frame number into------V----- 
  while ( (didreadframe = fscanf(fnsFP, "%d", &fwanted), didreadframe == 1)  )
    {
      //Must read to somewhere even though we throw some away
      //Maybe we'll develop double buffering if performance is bad.

      size_t rret = fread( yuvbuf, yuvsize, 1, yuvinFP );
      if ( rret != 1 ) {
	cerr << argv[0] << " stops. yuv stream ran out of frames when frame "
	     << fwanted << " was wanted. ???" << endl;
	break;  //out of reading wanted frame number loop
      }
      fcount++; //yup, we read a frame.
      
      if( fwanted == fcount ) {
	//We love you! 3 mutually exclusive use cases:
	if( do_yuv_stream_out ) {
	    size_t wret = fwrite( yuvbuf, yuvsize, 1, yuvoutFP );
	    if( wret != 1 ) {
	      error( 1, errno, "Failure to write frame %ul to output stream.", fwanted);
	    }
	}
	
	//in the other two, WE convert the yuv to a .bmp
	if( do_bmp_stream_out ) {
	  }

	if( do_bmp_dir_out ) {
	  }
      }
      
      else {
	//We discard the unwanted boring picture.
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
      {"yuv-out-stream-fd", required_argument, &do_yuv_stream_out, 1},  //1
      {"bmp-out-stream-fd", required_argument, &do_bmp_stream_out, 1},  //2
      {"bmp-out-dirpath", required_argument, &do_bmp_dir_out, 1},       //3
      {"bmp-conversion", required_argument, 0, 0 },                     //4
      {"bmp-name-prefix", required_argument, 0, 0},                     //5
      {0,         0,                 0,  0 }
    };
    c = getopt_long( *argc, *argv, "",
		    long_options, &option_index);
    if (c == -1)
      break;
    int ret;
    switch (c) {
    case 1:
      switch (option_index) { 
      case 0: break;
      case 1: 
	ret = sscanf(optarg, "%d", &yuvoutfd);
	if (ret != 1 )
	  {
	    error(1, 0, "--yuv-out-stream_fd %s BAD, arg should be a fd int",
		  optarg);
	  }
      	break;
      case 2:
	ret = sscanf(optarg, "%d", &bmpoutfd);
	if (ret != 1)
	  {
	    error(1, 0, "--bmp-out-stream_fd %s BAD, arg should be a fd int",
		  optarg);
	  }
      	break;
      case 3:
	bmpdirpath = optarg;
	break;
      case 4:
	bmpconversion = optarg;
	break;
      case 5:
	bmpnameprefix = optarg;
	break;
      }
    }
  }

  char *cmd = (*argv)[0];
  *argc = *argc - (optind - 1);
  *argv = *argv + (optind - 1);
  (*argv)[0] = cmd;
  return 0;
}
