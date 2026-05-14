/**
 yuvSelectFilter WidthxHeight framenums-fd yuvinput-fd [at least one option]

 Width, Height must be even.  
 fd's are small integers. Don't use 2 since stderr is used for verbose msgs and errors.
 (Typically, you choose and redirect a pipe or a file to them.)
 Frame numbering begins with 1.
 2nd input is a whitespaced list of strictly increasing frame numbers you want.
 3nd input is the sequence all of yuv frames, an I420p raw encoded movie.

There are 3 use cases specified by mutually exclusive options and their suboptions:

--yuv-out-fd <fd>  Outputs stream of yuv frames through file descriptor fd

--bmp-out-fd <fd> Outputs stream of 54-B header bmp frames through file descriptor fd
         [--bmp-conversion kind(TBD)]

--bmp-out-dirpath <writable dir name for .bmp files> [--bmp-conversion kind(TBD)]
         [--bmp-prefix myname  Written files are named mynamedddddd.bmp, not FRAMEdddddd.bmp]

optional options:
--verbose
--seline-fmt '..%d..' sscanf format for extracting one int from one frame selection data line,
   It must have a conversion specification for EVERY field in each line, so fscanf will scan over
   the whole line before the next line.  Thus %*<char> should be used to skip fields.
   EG. Use default '%d' for just frame numbers; 
    you can do --seline-fmt '%*d %d %*d %*d %*d %*f' for C-TAP .out file lines. 

  This program doesn't look inside yuv frames except to convert them to .bmps,
  but assumes their length is (3*width*height)/2, which is for I420p format.
 (So you can can psychodelic fun with random input.)
*/

#include <stdio.h>
#include <error.h>
#include <errno.h>
#include <iostream>
#include <getopt.h>
#include <stdint.h> //for bytes=uint8_t
using namespace std;

static char usage[] = "yuvSelectMult WidthxHeight framenums-fd yuvinput-fd [one --option required]\n\
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
static const char *selinefmt = "%d";
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
  if( argc != 4 ) //after --format arg processing, cmd + dims + wantedfd + inputfd
    {
      error(1, 0, usage);
    }
  
  optionprocess(); //Check consistency and do settings that vary with options.

  // 1st required arg: WWWWxHHHH 
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
  yuvbuf = new uint8_t[yuvsize];  //Big Mem buffer.
  if (! yuvbuf) {
    error(1, errno, "Cant allocate size %lu buffer.", yuvsize);
  }
  
  // 2nd required arg: frame number file fd
  if( 1 != (ret = sscanf(argv[2], "%d", &fnsfd) ) )
    {
      error(1, 0, "after WWWxDDD must be fd for file or PIPE of wanted frame numbers, not %s", argv[2]);
    }
  fnsFP = fdopen(fnsfd, "r");
  if ( ! fnsFP )
    {
      error(1,errno, "Cant open fd %d for reading fr. numbs. to stdio FILE.", fnsfd);
    }
  
  // 3rd required arg: yuv file or PIPE fd
  if( 1 != (ret = sscanf(argv[3], "%d", &yuvinfd) ) )
    {
      error(1, 0, "after fr. nums fd must be fd PIPE or file to input yuv frames %s", argv[2]);
    }
  yuvinFP = fdopen(yuvinfd, "r");
  if ( ! yuvinFP )
    {
      error(1,errno, "Cant open fd %d for reading yuvs to stdio FILE.", yuvinfd);
    }

  //Frame numbers will start at 1.
  int fcount = 0;
  int fwanted;
  int gotcount = 0;
  int didreadframe = 0;
  
  //Loop to read the next wanted frame number into------V----- 
  while ( (didreadframe = fscanf(fnsFP, selinefmt, &fwanted), didreadframe) == 1  ) {
    //Loop to read the next available frames up to and including the latest wanted one.
    while ( fwanted >= (fcount+1) ) {
      size_t rret = fread( yuvbuf, yuvsize, 1, yuvinFP );
      if ( rret != 1 ) {
	cerr << argv[0] << " stops. yuv stream ran only "<< fcount << " frames when frame "
	     << fwanted << " was wanted. ???" << endl;
	return 1;  //out of reading wanted frame number loop
      }
      fcount++; //yup, we read a frame.
      if(vb) cerr << "Did read frame " << fcount << endl;
      
      if( fwanted == fcount ) {
	//We love you! 3 mutually exclusive use cases:
	if( do_yuv_stream_out ) {
	  if(vb) cerr << "Try to output frame " << fcount << endl;
	  size_t wret = fwrite( yuvbuf, yuvsize, 1, yuvoutFP );
	  if( wret != 1 ) {
	    error( 1, errno, "Failure to write frame %ul to output stream.", fwanted);
	  }
	  else
	    if(vb) cerr << "Wrote frame " << fcount << endl;
	  gotcount++;
	}
	//in the other two, WE convert the yuv to a .bmp
	if( do_bmp_stream_out ) {
	}
	
	if( do_bmp_dir_out ) {
	}
      }
      else {
	if(vb) cerr << "Skip frame " << fcount << endl;
	//We ignore the unwanted boring picture, overpaint its space with next.
      }
    }
  }
  if(vb)
    {
      cerr << argv[0] << " Done. "
	   << fcount << " frames read. "
	   << gotcount << " frames selected. Bye." << endl;
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
      {"yuv-out-fd", required_argument, &do_yuv_stream_out, 1},  //1
      {"bmp-out-fd", required_argument, &do_bmp_stream_out, 1},  //2
      {"bmp-out-dirpath", required_argument, &do_bmp_dir_out, 1},       //3
      {"bmp-conversion", required_argument, 0, 0 },                     //4
      {"bmp-prefix", required_argument, 0, 0},                     //5
      {"seline-fmt", required_argument, 0, 0},                     //6
      {0,         0,                 0,  0 }
    };
    c = getopt_long( *argc, *argv, "",
		    long_options, &option_index);
    if (c == -1)
      break;
    int ret;
    switch (c) {
    case 0:
      switch( option_index ) {
      case 1:
	if(vb) cerr << "--yuv-out-fd arg=" << optarg << endl;
	ret = sscanf(optarg, "%d", &yuvoutfd);
	if(vb) cerr << "--yuv-out-fd int=" << yuvoutfd << endl;
	if (ret != 1 )
	  {
	    error(1, 0, "--yuv-fd %s BAD, arg should be a fd int",
		  optarg);
	  }
      break;
      case 2:
	ret = sscanf(optarg, "%d", &bmpoutfd);
	if (ret != 1)
	  {
	    error(1, 0, "--bmp-fd %s BAD, arg should be a fd int",
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
      case 6:
	selinefmt = optarg;
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
