#include "BlobGetter.h"
#include <fstream>
#include <iostream>
#include <string>
#include <getopt.h>
#include <error.h>
#include <errno.h>

static int get_our_options( int *argc, char **argv[]);
int vb = 0; //verbose
int width;   //dims of 
int height;  //frames
size_t size;
size_t bloblen;
int inyuvfd;
FILE* p1afp = 0;  //fd: input from Phase1a (one line/orig frame of decimated movie)
FILE* p1bfp = 0;  //fd: output from Phase1b (event frame# maxdiff x y prob) 
int outyuvfd; //output fd for raw yuv video.
char *pgm;
int main(int argc, char * argv[] )
{
  get_our_options(&argc, &argv); //lops off specified --options, just --verbose for now.
  pgm = argv[0];
  if( argc != 4 ) {
    error(1,0,"Call as %s wwwwxhhhh <file-desc for in yuv stream> <file-desc for out yuv stream> [opts]",
	  pgm);
  }
  if( (2 != sscanf(argv[1],"%dx%d",&width,&height)) ||
      (1 != sscanf(argv[2],"%d", &inyuvfd) ) ||
      (1 != sscanf(argv[2],"%d", &outyuvfd) ) )  {
    error(1,0,"Call as %s wwwwxhhhh <file-desc for in yuv stream> <file-desc for out yuv stream> [opts]",
	  pgm);
  }
  size = width*height;
  bloblen = size + size/2; //#bytes in a size pixel yuv I420p raw video frame
  uint8_t *buf = new uint8_t[bloblen];
  if( ! buf ) {
    error(1, errno, "%s cant alloc mem for one yuv frame", pgm);
  }

  FILE *inyuvfp = fdopen( inyuvfd, "r");
  BlobGetter BG( inyuvfp, bloblen);
  FILE *outyuvfp = fdopen( outyuvfd, "w" );
  int i = 1;
  while( 1 == BG.getith( buf, i ) ) {
    if( 1 != fwrite( buf, bloblen, 1, outyuvfp ) ) {
      error(1, errno, "%s cant write yuv frame %d.", pgm, i);
    }
    i++;
  }
  if( vb ) {
    fprintf(stderr, "%s wrote %d yuv frames to stream.", pgm, i);
      }
  fclose(inyuvfp);
  fclose(outyuvfp);
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
      {"verbose", optional_argument, &vb, 1},                     //0
      /*  {"yuv-out-fd", required_argument, &do_yuv_stream_out, 1},   //1
      {"bmp-out-fd", required_argument, &do_bmp_stream_out, 1},   //2
      {"bmp-out-dirpath", required_argument, &do_bmp_dir_out, 1}, //3
      {"bmp-conversion", required_argument, 0, 0 },               //4
      {"bmp-prefix", required_argument, 0, 0},                    //5
      {"seline-fmt", required_argument, 0, 0},                    //6
      {"offset", required_argument, 0, 0},                        //7
      {"compress", optional_argument, &do_cx, 1},                 //8
      //option must use = sign: --compress="cmd [args]"
      {"nkids", required_argument, 0, 0},                         //9
      */
      {0,         0,                 0,  0 }
    };
    c = getopt_long( *argc, *argv, "",
		    long_options, &option_index);
    if (c == -1)
      break;
    int ret;
    /*
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
	bmpdirpaths = string(optarg);
	break;
      case 4:
	bmpconversions = optarg;
	break;
      case 5:
	bmpnameprefixs = string(optarg);
	break;
      case 6:
	selinefmt = optarg;
	break;
      case 7:
	ret = sscanf(optarg, "%d", &offset);
	if(ret == 0)
	  {
	    error(1, 0, "badly formatted --offset %s.", optarg);
	  }
	if( offset < 0 )
	  {
	    error(1, 0, "Negative --offset %d not supported (yet).", offset);
	  }
	break;
      case 8:
	if( optarg ) {
	  (void) mk_cx_argv( optarg );
	}
	break;
      case 9:
	cx_maxkids = atoi(optarg);
	if( cx_maxkids <= 0 ) {
	  error(1, 0, "Badly formatted or negative max number %d of compressing children.\n", cx_maxkids);
	}
	break;
      }
    }
    */
  }

  char *pgm = (*argv)[0];
  *argc = *argc - (optind - 1);
  *argv = *argv + (optind - 1);
  (*argv)[0] = pgm;
  return 0;
}
