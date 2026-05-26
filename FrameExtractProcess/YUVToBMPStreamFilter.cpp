/**
 YUVToBMPStreamFilter width height < image.yuv > image.bmp [--verbose]

 image.yuv must be widthxheight dim I420p yuv.

 This is a drop-in replacement for the faulty version used in 
 early May 2026 TESTING/TestPipeOpt

*/
#include <string>
#include <stdio.h>
#include <error.h>
#include <errno.h>
#include <iostream>
#include <getopt.h>
#include <stdint.h> //for bytes=uint8_t
#include "bmp.h"
#include "yuvtobmpT.h"
#include <cassert>
#include <unistd.h> //for syscalls like read()
using namespace std;

static const char usage[] = "YUVToBMPStreamFilter Width Height < image.yuv > image.bmp\n";

//common data from command line arguments, since raw files don't have metadata
//we ONLY support I420p ones.
static char *cmd;  //will =argv[0], for messages.

static  unsigned int width, height;

// Streams (FILE *) instead of syscall fps so we take
// advantage of buffering, even though all reads and writes
// are sequential.



//options
static int vb = 0;                     //verbose

//FILE POINTERS
static FILE *yuvinFP = stdin;    //Already open.
static FILE *bmpoutFP = stdout; 

static class BMclass *pBM; //for when we make bmps

static int get_our_options( int *argc, char **argv[]);

static void optionprocess() { }

uint8_t *yuvbuf = 0;

int main(int argc, char *argv[]) {
  get_our_options(&argc, &argv); //lops off specified --options, just --verbose for now.

  if(vb) { cerr << "Hi. " << cmd << " is verbose" << endl; fflush(stderr); }

  optionprocess(); //(nothing for now). Check consistency and do settings that vary with options.

  int ret; //let's reuse, ugh.
  if( argc != 3 ) //after --format arg processing, cmd + width + height
    {
      cerr << endl << "argc=" << argc << endl << flush;
      for (int i = 0; i<argc; i++)
	cerr << "argv[" << i << "]=" << argv[i] << endl << flush;
      cerr << usage << endl << "Will crash now...hope I'm compiled for debugging.. wwwwoooo!!VvvvvBANG." << endl << flush;
      assert(0);
     }
  
  // 1st required arg: width 
  if ( 1 != (ret = sscanf(argv[1],"%u", &width)))
    {
      error(1, 0, "First param %s should be width, a decimal string.\n%s", argv[1], usage);
    }
  if ( 1 != (ret = sscanf(argv[2],"%u", &height)))
    {
      error(1, 0, "Second param %s should be height, a decimal string.\n%s", argv[1], usage);
    }
  if ( ((width & 1) != 0) || ((height & 1) != 0) )
    {
      error(1, 0, "width %d and height %d must be even (don't know what if not) for us."
	    "\n Though, %s doesn't look inside yuv frames",
	    width, height, argv[0]);
    }
  size_t yuvsize = (width*height*3)/2;

  pBM = new BMclass(width, height);
  
  //Seems I have to use logic from OneYUVtoOneBmp
  
  int firstc;
   //Apparently, Unix doesn't make feof(..) != 0 until
              //after we try to read!
  unsigned int imgCount = 0;
  while( (firstc = fgetc(yuvinFP)) != EOF )
    { //There is stuff to work on!
      ungetc(firstc, yuvinFP);

      if (yuvtobmpT( yuvinFP, pBM ) )  //gets w/h from *pBM,
      //no erronous return inplemented yet.
	{
	  cerr << "yuvtobmpT returned error." << endl << flush;
	}

      pBM->write(); //BMClass::write is overloaded. 
      imgCount++;
      if(vb) { fprintf(stderr, "\rImage%d", imgCount); fflush(stderr); }
    }
  
  fprintf(stderr, "%s transformed %d images.\n", cmd, imgCount); fflush(stderr);
  fclose(yuvinFP);
  fclose(stdout);
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
      {"verbose", no_argument, &vb, 1},       //0
      {0,         0,                 0,  0 }
    };
    c = getopt_long( *argc, *argv, "",
		    long_options, &option_index);
    if (c == -1)
      break;
    // int ret; //option processing pattern.
    // switch (c) { 
    //    case 0:
    // switch( option_index ) {
    //  case 1: break;
    //  }
    // }
  }

  cmd = (*argv)[0]; //for messages
  *argc = *argc - (optind - 1);
  *argv = *argv + (optind - 1);
  (*argv)[0] = cmd;
  return 0;
}
