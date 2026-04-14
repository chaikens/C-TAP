/**
   This version reads a concatenation of .bmp files from fd 0
*/

int stdinfreads = 0; //for debugging
int verbose = 0;


#include <string>
#include <cstring>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <error.h>
#include <errno.h>
#include <cmath>
#include <math.h>
#include <sstream>
#include <vector>

FILE* fd0 = 0;

typedef unsigned short pixCoord; 

#define Custom
/* When Custom is defined, 
   (1) global variable std::string camera = "Custom"

   (2) code in main() unconditionally 
       sets CROP_{X,Y}{I,F} from Camsett.txt

   (3) Mysterious (buggy?) line which does nothing is compiled
       before computing the differences:
   
      #ifdef Custom
	if ( false ) ExclusionZone = true;
      #endif
*/ 

#ifdef Custom
unsigned int CROP_YI = 0; //left edge
unsigned int CROP_XI = 0; //top edge
#else
unsigned int CROP_YI = 15;
unsigned int CROP_XI = 10;
#endif

#ifdef CamB1
unsigned int CROP_YF = 1275;
#else
unsigned int CROP_YF = 1249; //right edge (UFODAP: 1920)
#endif
#ifdef CamA1 //bottom edge is crop_xf. High num to catch boat: 635 MIN. 650 for mult frames
unsigned int CROP_XF = 590; static const std::string camera = "A1";
#endif
#ifdef CamA2
unsigned int CROP_XF = 640; static const std::string camera = "A2";
#endif
#ifdef CamA3
unsigned int CROP_XF = 590; static const std::string camera = "A3";
#endif
#ifdef CamA4
unsigned int CROP_XF = 575; static const std::string camera = "A4";
#endif
#ifdef CamB1
unsigned int CROP_XF = 580; static const std::string camera = "B1";
#endif
#ifdef CamB2
unsigned int CROP_XF = 590; static const std::string camera = "B2";
#endif
#ifdef CamB3
unsigned int CROP_XF = 575; static const std::string camera = "B3";
#endif
#ifdef CamB4
unsigned int CROP_XF = 590; static const std::string camera = "B4";
#endif
#ifdef Custom
unsigned int CROP_XF = 1080; /*default: UFODAP*/
static const std::string camera = "Custom";
#endif

using namespace std;

int Ret[2] = { 505, 85 };
int diffs[3] = { 0, 0, 0 };
unsigned int width, height, imgsizeb;

static unsigned char *BMP_A;
static unsigned char *BMP_B;
static int frameCnt = -1;

int readFirstBMPToAandAllocB()
{
  int i;
  /* GLOBAL! */
  fd0 = //stdin;
    fdopen(0, "r");
  // in case we make the pipe input more flexible and check errors.
  // if( !f ) {
  //   error(1, errno, "Opening first frame %s failed.", filename);
  // }
  unsigned char info[54];
  size_t readret;
  // read the 54-byte header
  fread(info, 54, 1, fd0);
  if(verbose) {
    stdinfreads++; fprintf(stderr, "%dth read\n", stdinfreads);
  }
  
  // extract image height and width from header
  width = *(int*)&info[18];
  height = *(int*)&info[22];
  //cerr << width << " " << height << endl;

  // allocate three bytes per pixel
  imgsizeb = 3 * width * height;
  BMP_A = new unsigned char[imgsizeb];
  BMP_B = new unsigned char[imgsizeb];

  // read the rest of the data at once
  readret = fread(BMP_A, imgsizeb, 1, fd0);
  if(verbose) {
    stdinfreads++; fprintf(stderr, "%dth read.\n", stdinfreads);
  }
  if( 1 != readret ){
    if( readret < 0 ) {
      error(1, errno, "First frame: Reading stdin failed.");
    }
      else {
	error(1, errno, "First frame: When reading image data, only %ld of %u read.", readret, imgsizeb);
      }
    }
  // don't close fd0!
  frameCnt = 1;
  
  for(int i = 0; i < imgsizeb; i += 3)
    {
      // flip the order of every 3 bytes
      unsigned char tmp = BMP_A[i];
      BMP_A[i] = BMP_A[i+2]; BMP_A[i+2] = tmp;
    }

  return 0;
}


int readSubsequentBMP(unsigned char *dest)
{
  //FILE* fd0 = stdin;
  //fd0 is now global
  //if( !fd0 ) {
  //  error(1, errno, "Opening %dth frame %s failed.", frameCnt+1, filename);
  //}
  unsigned char info[54];
  size_t readret;

  if(verbose) fprintf(stderr, "readSubs for header after %dth read\n", stdinfreads);
  
  // read the 54-byte header or detect EOF! feof != 0 means EOF.
  readret = fread(info, 54, 1, fd0);
  if(verbose){
    stdinfreads++; fprintf(stderr, "%dth read, \n", stdinfreads);
  }
  if( readret != 1){
    if(verbose) fprintf(stderr, "readret!=1, = %ld\n", readret);
    if( feof(fd0) != 0 ) {
      //There are no more bitmaps to process.
      // Should we clear the error?  I think so
      // since I hope we can process multiple movies
      // this way, which will entail replacing the
      // ffmpeg movie->bitmaps process and our pipe
      // from it.
      clearerr(fd0);
      return 1;
    }
    if( readret < 0 ) {
      error(1, errno, "Subsequent frames: Reading header from stdin failed but NOT from EOF!");
    }
    else {
	error(1, errno, "Subsequent frames: On reading header, only %ld of %u read.", readret, 54);
      }
    }
  
  // extract image height and width from header
  if( width   != *(int*)&info[18] ||
      height  != *(int*)&info[22]  ) {
    cerr << "DIFFERENT DIM of Subsequent bitmap " << endl;
    cerr << "First had width=" << width
	 << " height=" << height << endl;
    cerr << "But now!  width=" << *(int*)&info[18]
	 << " height=" << *(int*)&info[22] << endl;
    exit(1);
  }
  
  // read the rest of the data at once
  readret = fread(dest, imgsizeb, 1, fd0);
  if(verbose) {
    stdinfreads++; fprintf(stderr,"%dth read\n", stdinfreads);
  }
  if( 1 != readret ){
    if( readret < 0 ) {
      error(1, errno, "Subsequent frames: Reading a whole bitmaps data failed.");
    }
    else {
	error(1, errno, "Subsequent frames: Only %ld of %u read, wrong bitmap fmt", readret, imgsizeb);
      }
    }

  frameCnt++;
  
  for(int i = 0; i < imgsizeb; i += 3)
    {
      // flip the order of every 3 bytes
      unsigned char tmp = dest[i];
      dest[i] = dest[i+2]; dest[i+2] = tmp;
    }

  return 0; //So caller will know to try another.
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

static bool (*inExclusionZone)( pixCoord ii, pixCoord jj );
// One can code this variable be set when the program runs 
// by copying the pointer from the array below.

struct camera {
  const char *name;
  bool (*ezFun)(pixCoord, pixCoord);
} cameras[] = {
  { "None",  ezNone },
  { "CamA1", ezCamA1 },
  { "CamA2", ezCamA2 },
  { "CamA3", ezCamA3 },
  { "CamA4", ezCamA4 },
  { "CamB1", ezCamB1 },
  { "CamB2", ezCamB2 },
  { "CamB3", ezCamB3 },
  { "CamB4", ezCamB4 },
  { 0, 0 }  //to terminate a search loop, so we
  // can pass the camera name as an argument or
  // get it from CamSett.txt.
};
  
static bool ((* ezFunArray[])) (pixCoord, pixCoord)  =
  { ezNone, ezCamA1, ezCamA2, ezCamA3, ezCamA4,
    ezCamB1, ezCamB2, ezCamB3, ezCamB4 };

/**
  FLIRanalysisPhase1aCamXPipe.cpp, compiled to Phase1aPipe

  $ Phase1APipe start_frame_no. number_of_frames_to_process \
    < (pipe or file containing a concatenation of bitmaps).

  It writes to stdout a one line report for each adjacent 
  pairs of frames. The index at each line beginning
  is of the 2nd frame of the pair, so the first index
  is start_frame_no. + 1.  At present, our start_frame_no.
  is 0.

  If there are more frames than number_of_frames_to_process,
  processing stops after the last frame.

  Warning: This program modifies CamSett.txt by appending
  the camera name.

*/

int main ( int argc, char** argv ) {

  int camera_index = 0;
  inExclusionZone = cameras[camera_index].ezFun;
  /* make it point to the right function */

  if(argc < 2) {
    error(1, 0, "Phase1aPipe startnumber howmanyframes [Cloud Cover, not used].");
  }

  if(0 == strcmp(argv[1], "--verbose")) {
    verbose = 1;
    argc--;
    argv++; //yes, it's a hack
  }
      
  if(verbose) {
    fprintf(stderr, "Phase1aFilter Starting\n");
  }
    
  unsigned long start = (unsigned long)atof(argv[1]);
  unsigned long end = start + (unsigned long)atof(argv[2]);
  //arg[3] is unused!
  if( argc >= 4 ) {
    bool CC = atoi(argv[3]); //CC is unused??
  }  
  char line[80]; double temp; vector<double> CamSett;
  FILE *file = fopen("CamSett.txt","r");
  if( !file ) {
    error(1, errno, "CamSett.txt file missing from CWDir.");
  }
  for ( unsigned short k = 0; k < 20; ++k ) {
    fscanf ( file, "%s %lf", line, &temp );
    CamSett.push_back(temp);
  }
  fscanf ( file, "%s", line ); fclose(file);
  unsigned short MinThr = (unsigned short)CamSett[19], SubThr = (unsigned short)CamSett[10];
  if ( line[0] != 'c' ) {
    file = fopen("CamSett.txt","a");
    fprintf(file, "cameraName= %s\n", camera.c_str());
    fclose(file);
  }

#ifdef Custom
  CROP_XI = (unsigned int)CamSett[15];
  CROP_XF = (unsigned int)CamSett[16];
  CROP_YI = (unsigned int)CamSett[17];
  CROP_YF = (unsigned int)CamSett[18];
#endif

  unsigned long k = start;

  readFirstBMPToAandAllocB( );
  unsigned char* dataOld = BMP_B;
  unsigned char* dataNew = BMP_A;

  //We can terminate the loop either by specifying a
  //range of frame numbers, or detecting EOF after
  //processing the last pair.

  for ( k = start+1; k < end; ++k ) {
    unsigned char* tem = dataOld; dataOld = dataNew; dataNew = tem; 

    if( 0 != readSubsequentBMP (dataNew) )
      { //no more bitmaps
	break; //out of for( k = ... )
      }

    int maximum[3] = {-1,-1,-1}; int minimum[3] = {256,256,256}; int maxLoc[3][2] = {0}; int minLoc[3][2] = {0};
    int NumPixAbvThr[3][2] = {0};
    int NumPixAbvSubThrSum = 0, CloudCover = 100;
    
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
    

      fprintf(stdout,
"%lu\t\t%i  %i %i\t%i  %i %i\t%i  %i %i\t\t%i  %i %i\t%i  %i %i\t%i  %i %i\t\t%i %i %i\t%i %i %i\t\t%d\n",
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



