
#include <string>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <error.h>
#include <errno.h>
#include <cmath>
#include <math.h>
#include <sstream>
#include <vector>
#include <getopt.h>
using namespace std;

// Camera settings will eventually be systematized.
// For now, defining this macro makes only Custom be used.
#define Custom
/* When Custom is defined, 
   (1) global variable std::string camera = "Custom"

   (2) code in main() unconditionally 
       sets CROP_{X,Y}{I,F} from CamSett.txt

   (3) Mysterious (buggy?) line which does nothing is compiled
       before computing the differences:
   
      #ifdef Custom
	if ( false ) ExclusionZone = true;
      #endif
*/ 


// Command line options.  Set when, early, main calls
static int get_our_options( int *argc, char **argv[]);

static const char *bitmaps_dir = 0;
static const char *CamSett_file = 0;
static const char default_bitmaps_dir[] = "bitmaps";
static const char default_CamSett_file[] = "CamSett.txt";
static const char *pix_scale_string = 0;

//scaling? will use this everywhere including width and height
typedef uint16_t pixCoord;
//Maybe, someday, the pixCoord type will be a C++ class, with
//useful member functions and supporting better type checking.

//declare these globals undefined so we set them with a function
//instead of compile time selected initializers
pixCoord CROP_YI, //left edge
  CROP_XI, //top edge
  CROP_YF, //right edge
  CROP_XF; //bottom edge

//will be set by init_CROPS_and_camera() at runtime
string camera;

void init_CROPS_and_camera(void)
{
  //We have these preprocessor conditionals to
  //maintain regression as we develop support for
  //different cameras.

  //scaling?
#ifdef Custom
  CROP_YI = 0; //left edge
  CROP_XI = 0; //top edge
#else
  CROP_YI = 15;
  CROP_XI = 10;
#endif

  //Remember, camera is now a global string (C++ library) variable

  //CROP_YF is set here for everybody.
  //Camera particulars set CROP_XF below.
#ifdef CamB1
 CROP_YF = 1275;
#else
 CROP_YF = 1249; //right edge (UFODAP: 1920)
#endif

#ifdef CamA1 //bottom edge is crop_xf. High num to catch boat: 635 MIN. 650 for mult frames
 CROP_XF = 590; camera = "A1";
#endif
#ifdef CamA2
 CROP_XF = 640; camera = "A2";
#endif
#ifdef CamA3
 CROP_XF = 590; camera = "A3";
#endif
#ifdef CamA4
 CROP_XF = 575; camera = "A4";
#endif
#ifdef CamB1
 CROP_XF = 580; camera = "B1";
#endif
#ifdef CamB2
 CROP_XF = 590; camera = "B2";
#endif
#ifdef CamB3
 CROP_XF = 575; camera = "B3";
#endif
#ifdef CamB4
 CROP_XF = 590; camera = "B4";
#endif
#ifdef Custom
 CROP_XF = 1080; /*default: UFODAP*/
 camera = "Custom";
#endif
}

//scaling?
int Ret[2] = { 505, 85 }; //used ONLY by ezCamB1
int diffs[3] = { 0, 0, 0 };

//scaling?
pixCoord width, height;
uint32_t imgsize; //scaling? must accomodate Megas, 16 bit short no good.

static char * bmfilename( int f );

static uint8_t *BMP_A;
static uint8_t *BMP_B;
static int frameCnt = -1;

void readFirstBMPToAandAllocB(char* filename)
{
  int i;
  FILE* f = fopen(filename, "rb");
  if( !f ) {
    error(1, errno, "Opening first frame %s failed.", filename);
  }
  uint8_t info[54];
  size_t readret;
  // read the 54-byte header
  fread(info, sizeof(unsigned char), 54, f);

  // extract image height and width from header
  //scaling?
  width = *(pixCoord*)&info[18];
  height = *(pixCoord*)&info[22];
  //cerr << width << " " << height << endl;

  //scaling??
  // allocate three bytes per pixel
  imgsize = 3 * width * height;
  BMP_A = new uint8_t[imgsize];
  BMP_B = new uint8_t[imgsize];

  // read the rest of the data at once
  readret = fread(BMP_A, sizeof(uint8_t), imgsize, f);
  if( readret != imgsize){
    if( readret < 0 ) {
      error(1, errno, "Reading %s failed.", filename);
    }
      else {
	error(1, 0, "Only %ld of %u read, wrong bitmap fmt %s", readret, imgsize, filename);
      }
    }
  fclose(f);
  frameCnt = 1;
  
  for(int i = 0; i < imgsize; i += 3)
    {
      // flip the order of every 3 bytes
      unsigned char tmp = BMP_A[i];
      BMP_A[i] = BMP_A[i+2]; BMP_A[i+2] = tmp;
    }

  return ;
}


void readSubsequentBMP(char* filename, unsigned char *dest)
{
  size_t readret;
  FILE* f = fopen(filename, "rb");
  if( !f ) {
    error(1, errno, "Opening %dth frame %s failed.", frameCnt+1, filename);
  }
  uint8_t info[54];

  // read the 54-byte header
  readret = fread(info, sizeof(unsigned char), 54, f);
  if( readret != 54){
    if( readret < 0 ) {
      error(1, errno, "Reading header from %s failed.", filename);
    }
    else {
	error(1, 0, "Only %ld of %u read, wrong bitmap fmt? %s", readret, 54, filename);
      }
    }
  
  // extract image height and width from header
  //scaling?
  if( width   != *(int*)&info[18] ||
      height  != *(int*)&info[22]  ) {
    cerr << "DIFFERENT DIM of Subsequent bitmap " << filename << endl;
    cerr << "First had width=" << width
	 << " height=" << height << endl;
    cerr << "But now!  width=" << *(int*)&info[18]
	 << " height=" << *(int*)&info[22] << endl;
    exit(1);
  }
  
  // read the rest of the data at once
  readret = fread(dest, sizeof(unsigned char), imgsize, f);
  if( readret != imgsize){
    if( readret < 0 ) {
      error(1, errno, "Reading %s failed.", filename);
    }
    else {
	error(1, 0, "Only %ld of %u read, wrong bitmap fmt %s", readret, imgsize, filename);
      }
    }
  fclose(f);

  frameCnt++;
  
  for(int i = 0; i < imgsize; i += 3)
    {
      // flip the order of every 3 bytes
      uint8_t tmp = dest[i];
      dest[i] = dest[i+2]; dest[i+2] = tmp;
    }

  return ;
}



//
// Below, code any bool predicates for camera exclusion zones.
//
// Then, use it by assigning the function pointer variable
// inExclusionZone (static declared and defined to &ezNone below.
//

static bool ezNone( pixCoord ii, pixCoord jj ) {
  return false;
}

//scaling? all ez* functions!
static bool ezCamA1( pixCoord ii, pixCoord jj ) {
  return
    ( abs(jj-1205) < 40
      || (abs(jj-138) < 100 && abs(ii-30) < 25)
      || (abs(jj-1128) < 130 && abs(ii-70) < 70)
      || ((jj-637)*(jj-637)+(ii-363)*(ii-363)) < 10000
      );
}
static bool ezCamA2( pixCoord ii, pixCoord jj ) {
  return
    ( abs(jj-1120) < 100
      || (abs(jj-323) < 280 && abs(ii-39) < 40)
      || (abs(jj-1055) < 170 && abs(ii-49) < 50)
      || (abs(jj-650) < 115 && abs(ii-353) < 75)
      || ((jj-640)*(jj-640)+(ii-130)*(ii-130)) < 10000 );
}
static bool ezCamA3( pixCoord ii, pixCoord jj ) {
  return
    ( abs(jj-1163) < 100
      || (abs(jj-133) < 100 && abs(ii-30) < 25)
      || (abs(jj-1128) < 130 && abs(ii-36) < 35)
      || ((jj-635)*(jj-635)+(ii-357)*(ii-357)) < 10000 );
}
static bool ezCamA4( pixCoord ii, pixCoord jj ) {
  return
  ( abs(jj-1123) < 100
    || (abs(jj-323) < 278 && abs(ii-39) < 40)
    || (abs(jj-1055) < 170 && abs(ii-48) < 50)
    || ((jj-630)*(jj-630)+(ii-351)*(ii-351)) < 10000 );
}
static bool ezCamB1( pixCoord ii, pixCoord jj ) {
  return
    ( abs(jj-1123) < 98
      || (abs(jj-650) < 340 && abs(ii-Ret[0]) < Ret[1])
      || (abs(jj-323) < 278 && abs(ii-38) < 37)
      || ( jj > 1025 && ii > 565 )
      || (abs(jj-1058) < 163 && abs(ii-50) < 50)
      || ((jj-649)*(jj-649)+(ii-362)*(ii-362)) < 6400
      || ((jj-885)*(jj-885)+(ii-205)*(ii-205)) < 900
      || ((jj-408)*(jj-408)+(ii-205)*(ii-205)) < 529 );
}
static bool ezCamB2( pixCoord ii, pixCoord jj ) {
  return
    ( abs(jj-1205) < 45
      || (abs(jj-135) < 100 && abs(ii-28) < 27)
      || (abs(jj-1163) < 100 && abs(ii-37) < 34) );
}
static bool ezCamB3( pixCoord ii, pixCoord jj ) {
  return
  ( abs(jj-1135) < 85
    || (abs(jj-323) < 278 && abs(ii-40) < 36)
    || (abs(jj-1058) < 170 && abs(ii-70) < 25)
    || (abs(jj-640) < 115 && abs(ii-358) < 75) );
}
static bool ezCamB4( pixCoord ii, pixCoord jj ) {
  return
    (
     abs(jj-1203) < 45
     || (abs(jj-138) < 100 && abs(ii-30) < 25)
     || (abs(jj-1128) < 130 && abs(ii-65) < 65)
     || ((jj-637)*(jj-637)+(ii-364)*(ii-364)) < 11000 );
}

//scaling?
static bool (*inExclusionZone)( pixCoord ii, pixCoord jj );
// One can code this variable be set when the program runs 
// by copying the pointer from the array below.

static bool ((* ezFunArray[])) (pixCoord, pixCoord)  =
  { ezNone, ezCamA1, ezCamA2, ezCamA3, ezCamA4,
    ezCamB1, ezCamB2, ezCamB3, ezCamA4 };

int main ( int argc, char** argv ) {

  get_our_options( & argc, & argv);
  // --bitmaps-dir and --CamSett-file are really options.
  // if not supplied, the original defaults that work when
  // these files are in cwd are used.

  //This will probability have to go later, after (not yet)
  //variable cameras are supported.
  init_CROPS_and_camera();
  
  int camera_index = 0;
  inExclusionZone = ezFunArray[camera_index];
  /* make it point to the right function */
  
  unsigned long start = (unsigned long)atof(argv[1]);
  unsigned long end = start + (unsigned long)atof(argv[2]);
  //arg[3] is unused!
  if( argc >= 3 ) {
    bool CC = atoi(argv[3]); //Cloud Cover
  }  
  char line[80]; double temp; vector<double> CamSett;
  //At present, keys in the CamSett file, CamSett.txt
  //are NOT checked against the order of the file's lines!
  FILE *file = fopen(CamSett_file,"r");
  for ( unsigned short k = 0; k < 20; ++k ) {
    fscanf ( file, "%s %lf", line, &temp );
    CamSett.push_back(temp);
  }
  fscanf ( file, "%s", line ); fclose(file);
  unsigned short MinThr = (unsigned short)CamSett[19], SubThr = (unsigned short)CamSett[10];
  if ( line[0] != 'c' ) {
    file = fopen(CamSett_file,"a");
    fprintf(file, "cameraName= %s\n", camera.c_str());
    fclose(file);
  }

  //scaling?
  //note This code is in function main.
  //At present, THIS sets the CROPS, not our new initializer fn
#ifdef Custom
  CROP_XI = (pixCoord)CamSett[15];
  CROP_XF = (pixCoord)CamSett[16];
  CROP_YI = (pixCoord)CamSett[17];
  CROP_YF = (pixCoord)CamSett[18];
#endif

  unsigned long k = start;

  readFirstBMPToAandAllocB( bmfilename(k+1) );
  unsigned char* dataOld = BMP_B;
  unsigned char* dataNew = BMP_A;

  
  for ( k = start+1; k < end; ++k ) {
    unsigned char* tem = dataOld; dataOld = dataNew; dataNew = tem; 

    readSubsequentBMP ( bmfilename(k+1), dataNew );

    int maximum[3] = {-1,-1,-1}; int minimum[3] = {256,256,256}; int maxLoc[3][2] = {0}; int minLoc[3][2] = {0};
    int NumPixAbvThr[3][2] = {0};
    int NumPixAbvSubThrSum = 0, CloudCover = 100;

    //scaling? ii and jj too
    for ( int i = (height-1-CROP_XI); i >= (height-CROP_XF); --i ) {
      for ( int j = CROP_YI; j < CROP_YF; ++j ) {
	
	int rgbColorNew[3], rgbColorOld[3];
	
	
	rgbColorOld[0] = (int)dataOld[3 * (i * width + j) + 0];
	rgbColorOld[1] = (int)dataOld[3 * (i * width + j) + 1];
	rgbColorOld[2] = (int)dataOld[3 * (i * width + j) + 2];
	
	
	rgbColorNew[0] = (int)dataNew[3 * (i * width + j) + 0];
        rgbColorNew[1] = (int)dataNew[3 * (i * width + j) + 1];
        rgbColorNew[2] = (int)dataNew[3 * (i * width + j) + 2];
	
	diffs[0] = rgbColorNew[0]-rgbColorOld[0];
	diffs[1] = rgbColorNew[1]-rgbColorOld[1];
	diffs[2] = rgbColorNew[2]-rgbColorOld[2];
	int jj = j;
	int ii = height - 1 - i;

	//scaling? ezFuns rely on it.
	if ( !(*inExclusionZone)(ii, jj) || k == start ) {
	  if ( diffs[0] > maximum[0] )
	    { maximum[0] = diffs[0]; maxLoc[0][0] = ii; maxLoc[0][1] = jj; if ( maximum[0] > MinThr ) ++NumPixAbvThr[0][0]; if ( maximum[0] > SubThr ) ++NumPixAbvSubThrSum; }
	  if ( diffs[1] > maximum[1] )
	    { maximum[1] = diffs[1]; maxLoc[1][0] = ii; maxLoc[1][1] = jj; if ( maximum[1] > MinThr ) ++NumPixAbvThr[1][0]; if ( maximum[1] > SubThr ) ++NumPixAbvSubThrSum; }
	  if ( diffs[2] > maximum[2] )
	    { maximum[2] = diffs[2]; maxLoc[2][0] = ii; maxLoc[2][1] = jj; if ( maximum[2] > MinThr ) ++NumPixAbvThr[2][0]; if ( maximum[2] > SubThr ) ++NumPixAbvSubThrSum; }
	  if ( diffs[0] < minimum[0] )
	    { minimum[0] = diffs[0]; minLoc[0][0] = ii; minLoc[0][1] = jj; if ( minimum[0] <-MinThr ) ++NumPixAbvThr[0][1]; if ( minimum[0] <-SubThr ) ++NumPixAbvSubThrSum; }
	  if ( diffs[1] < minimum[1] )
	    { minimum[1] = diffs[1]; minLoc[1][0] = ii; minLoc[1][1] = jj; if ( minimum[1] <-MinThr ) ++NumPixAbvThr[1][1]; if ( minimum[1] <-SubThr ) ++NumPixAbvSubThrSum; }
	  if ( diffs[2] < minimum[2] )
	    { minimum[2] = diffs[2]; minLoc[2][0] = ii; minLoc[2][1] = jj; if ( minimum[2] <-MinThr ) ++NumPixAbvThr[2][1]; if ( minimum[2] <-SubThr ) ++NumPixAbvSubThrSum; }
	}
	
      } /* end pixel y loop */
    } /* end pixel x loop */
    
    //scaling? OUTPUT is in pixel coords.
      printf("%lu\t\t%i  %i %i\t%i  %i %i\t%i  %i %i\t\t%i  %i %i\t%i  %i %i\t%i  %i %i\t\t%i %i %i\t%i %i %i\t\t%d\n",
	      k,
	      maximum[0],maxLoc[0][1],maxLoc[0][0],
	      maximum[1],maxLoc[1][1],maxLoc[1][0],
	      maximum[2],maxLoc[2][1],maxLoc[2][0],
	      minimum[0],minLoc[0][1],minLoc[0][0],
	      minimum[1],minLoc[1][1],minLoc[1][0],
	      minimum[2],minLoc[2][1],minLoc[2][0],
	      NumPixAbvThr[0][0],NumPixAbvThr[1][0],NumPixAbvThr[2][0],
	      NumPixAbvThr[0][1],NumPixAbvThr[1][1],NumPixAbvThr[2][1],
	      NumPixAbvSubThrSum);

    
  } /* end of frame loop */
  
  return 0;
  /* We don't free BMP_A and BMP_B memory since we exit 1a's process right away.*/
  /* Yes, we will skip frames when the caller asks for a partial job! */
  /* Each output line is numbered by it's OLD frame. */
} /* end of main */

string bmp_file_printf_format_string; //set by get_our_options
const char *bmp_file_printf_format_cstring; //set by get_our_options

string bmpfilename_string;             //set by get_our_options
string thumb_format = "thumb0%05d.bmp";
char *bmpfilename_buffer;              //set by get_our_options

static char * bmfilename( int f )
{
  //static char fn[] = "bitmaps/thumb000000.bmpXXXXXXXXXXXXXXXXXXXXXX";
  //sprintf(fn,"bitmaps/thumb0%05d.bmp", f);
  //return fn;
  sprintf(bmpfilename_buffer, bmp_file_printf_format_cstring, f);
  return bmpfilename_buffer;
}

//scaling?  We could pass a scale option.
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
      {"bitmaps-dir", required_argument,   0,  0 },
      {"CamSett-file", required_argument, 0,  0 },
      {"pix-scale", required_argument, 0,  0 },  
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
	break;
      case 3: pix_scale_string = optarg;
      }
    }
  }

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
  //  fprintf(stderr, "new_CamSett_file %s\n", CamSett_file);
  }
  else {
    CamSett_file = default_CamSett_file;
  }

  string bitmaps_dir_string = bitmaps_dir;
  bmp_file_printf_format_string = bitmaps_dir_string + "/" + thumb_format;
  bmp_file_printf_format_cstring = bmp_file_printf_format_string.c_str();
  bmpfilename_buffer =
    new char[bmp_file_printf_format_string.length()
	     + 2*5/*for safe paranoia*/];

  return 0;
}
