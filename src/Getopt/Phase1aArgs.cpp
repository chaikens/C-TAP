#include <iostream>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h> /* for exit */


static const char *bitmaps_dir = 0;
static const char *CamSett_file = 0;
static const char default_bitmaps_dir[] = "bitmaps";
static const char default_CamSett_file[] = "CamSett.txt";

static int get_our_options( int *argc, char **argv[]);

int main(int argc, char *argv[])
{
  get_our_options( & argc, & argv );

  printf("bitmaps_dir=%s\n", bitmaps_dir);
  printf("CamSett_file=%s\n", CamSett_file);
  
  for( int i = 0; i < argc; i++)
    printf("argv[%d]=%s\n", i, argv[i]);
  return 0;
}

static int get_our_options( int *argc, char **argv[])
{
  extern char *optarg; //globals from getopt and getopt_long
  extern int optind, opterr, optopt;

  int c;
  int digit_optind = 0;
  //so original argument getting works

  //Logic here adapted from man 3 getopt
  while (1) {
    int this_option_optind = optind ? optind : 1;
    int option_index = 0;
    static struct option long_options[] = {
      {"bitmaps-dir", required_argument,   0,  0 },
      {"CamSett-file", required_argument, 0,  0 },
      {0,         0,                 0,  0 }
    };
    c = getopt_long( *argc, *argv, "",
		    long_options, &option_index);
    if (c == -1)
      break;

    switch (c) {
    case 0:
      switch (option_index) {
      case 0: bitmaps_dir = optarg;
	break;
      case 1: CamSett_file = optarg;
      }
    }
  }

  printf("optind=%d argv[optind]=%s\n", optind, (*argv)[optind]);
  
  char *cmd = (*argv)[0];
  *argc = *argc - (optind - 1);
  *argv = *argv + (optind - 1);
  (*argv)[0] = cmd;

  if(bitmaps_dir) {
    fprintf(stderr, "new_bitmaps_dir %s\n", bitmaps_dir);
  }
  else {
    bitmaps_dir = default_bitmaps_dir;
  }
  
  if(CamSett_file) {
    fprintf(stderr, "new_CamSett_file %s\n", CamSett_file);
    CamSett_file = default_CamSett_file;
  }
    
  return 0;
}


