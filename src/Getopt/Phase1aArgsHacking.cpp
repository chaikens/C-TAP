#include <iostream>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h> /* for exit */

extern char *optarg;
extern int optind, opterr, optopt;

int main(int argc, char *argv[])
{

  char *new_bitmaps_dir = 0;
  char *new_CamSett_file = 0;
  int c;
  int digit_optind = 0;

  while (1) {
    int this_option_optind = optind ? optind : 1;
    int option_index = 0;
    static struct option long_options[] = {
      {"bitmaps-dir", required_argument,   0,  0 },
      {"CamSett-file", required_argument, 0,  0 },
      {0,         0,                 0,  0 }
    };

    
    c = getopt_long(argc, argv, "abc:d:012",
		    long_options, &option_index);
    if (c == -1)
      break;

    printf("c=getopt_long_ret=%d\n", c);
    
    switch (c) {
    case 0:
      printf("optind=%d\n", option_index);
      switch (option_index) {
      case 0: new_bitmaps_dir = optarg;
	break;
      case 1: 	new_CamSett_file = optarg;
      }
      printf("option %s", long_options[option_index].name);
      if (optarg)
	printf(" with arg %s", optarg);
      printf("\n");
      break;

    case '0':
    case '1':
    case '2':
      if (digit_optind != 0 && digit_optind != this_option_optind)
	printf("digits occur in two different argv-elements.\n");
      digit_optind = this_option_optind;
      printf("option %c\n", c);
      break;

    case 'a':
      printf("option a\n");
      break;

    case 'b':
      printf("option b\n");
      break;

    case 'c':
      printf("option c with value '%s'\n", optarg);
      break;

    case 'd':
      printf("option d with value '%s'\n", optarg);
      break;

    case '?':
      break;

    default:
      printf("?? getopt returned character code 0%o ??\n", c);
    }
  }

  printf("at end of loop, optind=%d\n", optind);
  for( int i = 0; i < argc; i++)
    printf("argv[%d]=%s\n", i, argv[i]);

 
  
  if (optind < argc) {
    printf("non-option ARGV-elements: ");
    while (optind < argc)
      printf("%s ", argv[optind++]);
    printf("\n");
  }
  char *cmd = argv[0];
  argc = argc - (optind - 1);
  argv = argv + (optind - 1);
  argv[0] = cmd;

  printf("New argc=%d\n", argc);

  for( int i = 0; i < argc; i++)
    printf("New arg[%d]=%s\n", i, argv[i]);

  printf("\n\n");

  if( new_bitmaps_dir ) printf("new_bitmaps_dir %s\n", new_bitmaps_dir);
  if( new_CamSett_file )printf("new_CamSett_file %s\n", new_CamSett_file);
    

  
  exit(EXIT_SUCCESS);
}

