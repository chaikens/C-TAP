#include "BlobGetter.h"
#include "MyYUV.h"
#include <fstream>
#include <iostream>
#include <string>
#include <getopt.h>
#include <error.h>
#include <errno.h>
#include <stddef.h>
#include <unistd.h>

static int get_our_options( int *argc, char **argv[]);
int vb = 0; //verbose
int width;   //dims of 
int height;  //frames
size_t npix;
size_t bloblen;
int inyuvfd;
FILE* p1afp = 0;  //fd: input from Phase1a (one line/orig frame of decimated movie)
FILE* p1bfp = 0;  //fd: output from Phase1b (event frame# maxdiff x y prob)

int offset = 0; //select ith frame where i=requested+offset

float cradius = 10.0;
float cthick = 2.0;
int outyuvfd; //output fd for raw yuv video.
char *pgm;
int main(int argc, char * argv[] )
{
  get_our_options(&argc, &argv); //lops off specified --options, just --verbose for now.
  pgm = argv[0];
  if( argc != 2 ) {
    error(1,0,"Call as %s wwwwxhhhh  3< yuv-input 4> yuv-output  [opts]",
	  pgm);
  }
  if( 2 != sscanf(argv[1],"%dx%d",&width,&height) ) {
    error(1,0,"Call as %s wwwwxhhhh <file-desc for in yuv stream> <file-desc for out yuv stream> [opts]",
	  pgm);
  }
  npix = width*height;
  bloblen = npix + npix/2; //#bytes in a size pixel yuv I420p raw video frame
  uint8_t *buf = new uint8_t[bloblen];
  if( ! buf ) {
    error(1, errno, "%s cant alloc mem for one yuv frame", pgm);
  }

  MyYUV YUV(width, height, buf);
  FILE *inyuvfp = fdopen(3,"r");
  BlobGetter BG( inyuvfp, bloblen);
  FILE *outyuvfp = fdopen(4,"w");
  int i = 1;

  int p1b_event_no;
  int p1b_frame_no;
  int p1b_diff;
  int p1b_x;
  int p1b_y;
  float p1b_prob;
  size_t p1b_scanf_ret;
  if(p1bfp) {
    p1b_scanf_ret=fscanf(p1bfp, "%d %d %d %d %d %f", &p1b_event_no, &p1b_frame_no, &p1b_diff, &p1b_x, &p1b_y, &p1b_prob);
  }
  
  while( 1 == BG.getith( buf, i ) ) {
    //cerr << "Calling Circle " << width/2 << " " << height/2 << endl;
    if( ! p1bfp ) {
      

      //YUV.circle( width/2, height/2, 20, 4 );
      if( 1 != fwrite( buf, bloblen, 1, outyuvfp ) ) {
	error(1, errno, "%s cant write yuv frame %d.", pgm, i);
      }
    i++;
    }
    else { //Yes, we want to select and draw circles!
      if( i == p1b_frame_no+offset ) {
	//one we want
	YUV.circle( p1b_x, p1b_y, 20, 3 );
	if( 1 != fwrite( buf, bloblen, 1, outyuvfp ) ) {
	  error(1, errno, "%s cant write yuv frame %d.", pgm, i);
	}
	p1b_scanf_ret = fscanf(p1bfp, "%d %d %d %d %d %f", &p1b_event_no, &p1b_frame_no, &p1b_diff, &p1b_x, &p1b_y, &p1b_prob);
	if(6 != p1b_scanf_ret ) { //we're probably out of .out lines
	  if(feof(p1bfp)==0) { //somethings wrong
	    cerr << "BabyFilter cant read another Phase1b line, but the FILE is not EOF???" << endl;
	  }
	  break;
	}
      }
      else {
	// skip it
      }
      i++;
    }
    
  }
  if( vb ) {
    fprintf(stderr, "%s wrote %d yuv frames to stream.", pgm, i);
      }
  if(p1bfp)
    {
      fclose(p1bfp);  //be nice to system
    }
  fclose(inyuvfp);
  close(3);
  fclose(outyuvfp);
  close(4);
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
      {"verbose", optional_argument, &vb, 1},              //0
      {"phase-1b-file", required_argument, 0, 0},          //1
      {"phase-1a-file", required_argument, 0, 0},          //2
      {"circle-radius", required_argument, 0, 0},          //3
      {"circle-border-thickness", required_argument, 0, 0}, //4
      {"offset", required_argument, 0, 0},                 //5
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
	p1bfp = fopen(optarg, "r");
	if( ! p1bfp ) {
	  error(1,errno, "BabyFilter cant open phase-1b-file %s", optarg);
	}
	break;
      case 2:
	p1afp = fopen(optarg, "r");
	if( ! p1afp ) {
	  error(1,errno, "BabyFilter cant open phase-1a-file %s", optarg);
	}
	break;
      case 3:
	if( 1 != sscanf(optarg, "%f", &cradius) ) {
	  error(1,0,"BabyFilter cant convert circle-radius %s to float.", optarg);
	}
	break;
      case 4:
	if( 1 != sscanf(optarg, "%f", &cthick) ) {
	  error(1,0,"BabyFilter cant convert circle-border-thickness %s to float.", optarg);
	}
	break;
      case 5:
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
       }
    }
  }

  char *pgm = (*argv)[0];
  *argc = *argc - (optind - 1);
  *argv = *argv + (optind - 1);
  (*argv)[0] = pgm;
  return 0;
}
