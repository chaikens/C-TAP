/**  FLIRanalysisPhase1aCamXPipeOpt.cpp, compiled to Phase1aPipeOpt

/////////////////////in the Pipe version editing in progress://///////////

   $ Phase1APipeOpt --pipeline StartFrameNumber NumberOfFrames CloudCover(not used)
     < (pipe or file containing a concatenation of bitmaps).
     > (pipe or file or maybe tee for .int result)

   $ Phase1aPipeOpt StartFrameNumber NumberOfFrames CloudCover(not used)
     > (pipe or file or maybe tee for .int result)
    It reads files named successively thumb000sss.bmp, ... from the bitmaps
    dir, where sss is the StartFrameNumber, say here of 
    exactly 3 decimal digits.
    The default bitmaps dir is ./bitmaps which the --bitmaps-dir changes

   Other common options:
   [--movie-scale M][--pixproc-scale P]--user-scale U]
    See scaling below.
  
   [--no-crop]  Loop over all pixels is unrestricted 
      (exclusion zones still apply.)

   [--bitmaps-dir dir] When --pipeline is NOT given, THEN frames are input 
                 from a directory of thumbdddddd.bmp
                 flies rather than stdin.
                 This overrides the default directory ./bitmaps

                 Good when a large, fast filesystem is used in addition
                 to the holder of our software, or when repeated analysis
                 should be done on the same extracted frames.

   [--CamSett-file file] makes it read parameters from a given 
                  CamSett.txt format file.  Otherwise, it reads 
                  ./CamSett.txt from the current working dir.  (It
                   no longer rewrites file by appending the camera name.)

                    Overrides default ./CamSett.txt  See documenatation
                    and reports from Phase1b.   
                    Settings architecture still under development.

   [--verbose]   Used for debugging.

   It writes to stdout a one line report for each adjacent 
   pairs of frames. The index at each line beginning
   is of the 2nd frame of the pair, so the first index
   is start_frame_no. + 1.  The current script calls this
   program once (per movie) with start_frame_no. = 0.

   Also, error reports written to stderr before exit.

   If there are more frames than number_of_frames_to_process,
   processing stops after the last frame.

    WHEN the --pipeline option is given, version reads a concatenation of
   .bmp (54 byte header) files 
    from fd 0.  It finishes either after processing 
    NumberOfFrames or EOF on fd 0. A double buffered (although 
    single thread) strategy is used, so each image is read only once.
    Memory use does not grow with number of frames.

    It gets the (width)x(height) resolution from the first image and fails
    if a subsequent image has different resolution.

    There are no other external non-standard dependencies or effects.
//////////////////////////////////////////////////////////////////////
*/

/**
/////////////////////scaling stuff///////////////////////////////////
 new scaling parameters!
 --movie-scale Mscale scale for original movie (or group) linear resolution
 --pixproc-scale Pscale for pixel processing 
 --user-scale  Uscale for specifying cropping, exclusion regions and
                          maybe eventually data analysis

 The scaling issue was raised because the DroneCalib1 movie
 is in 3840x2080 resolution, original C-TAP extracted half-resolution
 1920x1040 frames, and Custom (hardcoded and coded in CamSett.txt)
 selected clipping coordinates were
 specified in terms of the lower resolution frame coordinates.
 So, when we were able to process the movie with full resolution
 frames using pipelined extraction, the clipping cooridinates
 had to be doubled.  This is implemented below by the above scaling parmeters.

 In this case, the global UtoPmult=2.
 (Generally, UtoP=Pscale/Uscale which currently must have no roundoff.).
 The function UtoP(c) returns UtoPmult*c.
 Finally, the hard coded constants (among others) are first passed through
 UtoP(), in


//static bool ezDroneCalib1( pixCoord ii, pixCoord jj ) {
//  return (ii < UtoP(73) ) && (jj > UtoP(1568) );

// The CROP_[XY][IF] are user parameters.  The original code
// compared Pixel coord. with these CROP values.  Therefore,
// for the scaling version, we instead do the comparisons with
// UtoP(CROP..) values

// example of coded parameters.  We leave this alone.
//CROP_YF = 1249 //restricting more than 1920
//#ifdef Custom
//CROP_XF = 1080
//
// but the pixel loop limits are
// horizontal dir [0 <= UtoP(CROP_YI) <= x <= UtoP(CROP_YF) < width)
// vertical dir [0 <= UtoP(CROP_XI) <= y <= UtoP(CROP_XF) < height)
// (We may in the future rename the crop parameters.)


Thus, the May 2026 4-way comparison,
 HalfDecimated : --movie-scale 1 --pixproc-scale 1 --user-scale 1
 Full          : --movie-scale 2 --pixproc-scale 2 --user-scale 1
Remember, the original and current C-TAP half-scaled the movie and
used the resulting scaled pixels for crop specifications, exclusion zones
and result .int and .out files.

 Now, we'll try out the idea that:
 There should be 3 types for coordintes:
 MCoord -- used in the job's input movie files
 UCoord -- users specify crop limits and exclusion zones
           and similar features in terms of these
 PCoord -- Pixel or Processing Coordinates, used in
           individual processing stages, for now, Phase1a and b.
Eventually these might be implemented by C++ classes, say
with a common parent named Coord; but, most importantly,
each contains a common CoordInfo object that encodes
how the unsigned int (uint16) internal data in each object, as an instance
of which of the 3 classes it belongs to, should be converted
(by some sort of scaling) to objects of one of the other classes.

 But for now,
 scaleCD will be a keyword in comments about what conversions
should apply.  So, we can test and scope out a good, flexible object
oriented design.  No new types will be introduced as we try to
finish the April 2026 4-way comparison project on 3840x2080 movies,
with properties given by camera name Custom

Unfortunately, what we aim for April 2026 is .out files in terms of
the Pcoord's of Stage1b, used as such for the baby movies built out
our extracted (full or half resolution) movies, and will be double
the full frame runs compared to the half-frame runs.

                            LOOK AT LINES BELOW IN A 150-char WIDE SCREEN

                                                     concept----V         V------execution
 __Current__(so I can change now!) CamSett and excl zone params:Ucoord(=(1/2)Mcoord in April 2026):


                             concept----V         V------execution
    [cropping &/or exclus zones]---->Ucoord(=(1/2)Mcoord)--->V              /-->Matt      /-->Matt
Movie-->Mcoord--->Extract(1=Full OR 1/2=Half)--->Pcoord--->Phase1a--->Pcoord/----->Phase2/------->C-TAP,ImageMagik,ffmpeg---->Pcoord--->BabyMovie
[.bmp  ext OR YUV-pipe]/                     \---Pcoord----------------------------------------------^
 so soooo Phase1a HERE should think what we hard-coded in our ezDroneCalib1()
 are Ucoords!!
So, also, in this primative time, the .int, .out and Baby outputs are in Pcoords, NOT Ucoords.
If the .bmp/.yuv pipe consistency continues to manifest, we will have to straightforwardly translate the Pcoords in .int and .out
to some common Ucoord, for the future machine learning level work.

 A little background: When I got the .bmp, Fullframe try working on DroneShort1, I had to use an exclusion zone
 around the date/time display in order to get a better result; whilst no need for Half-frame--this might have done cropping,
 don't know for sure.  In the full frame try, FLIR got badly distracted by the date-time display. 

 We really want to vary Processing resolutions.  Mcoords don't change.
  So, Mcoord <---> Pcoord must vary at runtime
 So, if we want Ucoords to be fixed, Pcoord<---->Ucoord must vary at runtime.

 In this case, the global UtoPmult=2.
 (Generally, UtoP=Pscale/Uscale which currently must have no roundoff.).
 The function UtoP(c) returns UtoPmult*c.

 Finally, the hard coded constants (among others) are first passed through
 UtoP(), in

//static bool ezDroneCalib1( pixCoord ii, pixCoord jj ) {
//  return (ii < UtoP(73) ) && (jj > UtoP(1568) );

//UtoP(CROP_YF) = UtoP(1249) //restricting more than 1920
//#ifdef Custom
//UtoP(CROP_XF) = UtoP(1080)

*/

int stdinfreads = 0; //for old debugging of pipelined.

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
#include <getopt.h>
#include <cstring>
using namespace std;

int vb = 0; //verbose bool
FILE* fpin = stdin; //Might change, default ok when we are a filter.

// Camera settings will eventually be systematized.
// For now, defining this macro makes only Custom be used.
#define Custom
/* When Custom is defined, 
   (1) global variable std::string camera = "Custom"

   (2) code in main() unconditionally 
       sets CROP_{X,Y}{I,F} from CamSett.txt.
       See comments abt CROP. scaleCD

   (3) Mysterious (buggy?) line which does nothing is compiled
       before computing the differences:
   
      #ifdef Custom
	if ( false ) ExclusionZone = true;
      #endif
*/ 


char *progname; //for error and other messages
// Command line options.  Set when, early, main calls
static int get_our_options( int *argc, char **argv[]);

//options might change defaults:
static const char *bitmaps_dir = 0;
static const char *CamSett_file = 0;
static const char default_bitmaps_dir[] = "bitmaps";
static const char default_CamSett_file[] = "CamSett.txt";
int no_crop = 0; //scaleCD  //Should we keep this?


//scaling? will use this everywhere including width and height
typedef uint16_t pixCoord; //scaleCD
//
//Maybe, someday, the pixCoord type will be a C++ class, with
//useful member functions and supporting better type checking.
//

int cropargs = 0; //--crop-args option forces CROP_[X,Y][I,F]
                  //get_our_options cases counts this up to 4.
pixCoord argCROP_XI, argCROP_XF, argCROP_YI, argCROP_YF;
                  //values to be gotton from arguments


int pipeline = 0;  //Default is legacy bmp reading version.
                 //Set by --pipeline option.

//might be modified by get_our_options()
pixCoord Mscale = 1; //original movies
pixCoord Pscale = 1; //pixel processing
pixCoord Uscale = 1; //user specification of clipping and exclusion zones

//Comments and choices for now, during the Apr 2026 4-way comparison project:


//Design issues: Should cropping and exclusion zone specifying
//be in terms of the original movie collection pixel coodinates,
//or a resolution-free floating point coordinate system, like
//  [0.0, 1.0] X [0.0, AspectRatio]
//
// Another choice, handle integer data like (width,height)=(w,h) as
// homogeneous coods.
// That means (sw,sh) is equivalent to (tw,tw) for
// "suitable" s, t. (Perhaps that means s, t have enough divisors so
// (w/s)*s = (w/t)*t  and (h/s)*s = (h/t)*t etc.?)
//
// Or perhaps (w, h, s) actual homogeneous coords; carry s around.
// That will really simplify and make more reliable calculations
// for non-rectilinear exclusion zones!  (Don't think so for now
// except internal to ezfun coding; since only pixel coords are
// passed to later phases
//

//For the April 2026 comparison project:
// The below 4 CROP_* varable values will in the scale of the
//coord system used in the .bmp images Phase1a is processing.
//
//The unsigned constant, program parameter, or Sett
//file values before comparisons will be in the HALF SCALING our
//movies' original coordinate system, because that is what the
//earliest C-TAP software did.  It used ffmpeg to extract half-width/height
//frames for in order to limit .bmp frame storage to feasible
//capacities.  
//The handy function UtoP() be coded around every User coordinate
//to scale it for comparison to pixCoords in the .bmp 
//

//scaleCD
//By the way, halving the resolution of the images
// will 1/4 the \Theta(length*width) runtime of Phase1a.
//


// CROP_* variables 
//scaleCD
//Decision remade: they are Uscale.
// The constants and CamSett data used to set them are first scaled
// from Uscale to Pscale before USE.

//Declare these globals undefined so we set them with a function
//instead of compile time selected initializers
//
//Caution: The current names clash with some ituition about horiz vs vertical.
//  We clarify
//
// The Pixel coordinate system in which these are interpreted
// is that of Microsoft .bmp images:
// 0 <= x < width,  [0,width)  is the range of horiz. coods. LEFT to RIGHT
// 0 <= y < height, [0,height) is the range of verti. coods. BOTTOM to TOP
//
// The Pixel coordinate system used for Phase1.out lines uses the
// more conventional TOP to BOTTOM ordering for the vertical cooridinate.

pixCoord CROP_YI, /*LEFT edge  Initial HORIZONTAL COORDINATE, its for the
		  //vertical line that's the left edge inclusively
		  //of the (i.e., closed) effective, non-cropped region.
		  //CROP_YI == x is the eq. of the LEFT edge
		  */
  CROP_YF, /*RIGHT edge.   Final HORIZONTAL COORDINATE, its for the
	   //vertical line that's the right edge inclusively
	   //of the (closed) effective, non-cropped region.
	   //CROP_YF == x is the eq. of the RIGHT edge
	   */
  CROP_XI, /*TOP edge  Initial VERTICAL COORDINATE, its for the
	 //horiz  line that's the TOP edge inclusively
	 //of the (closed) effective, non-cropped region.
	 //CROP_XI == y is the eq. of the TOP edge
	 */
  CROP_XF; /* BOTTOM edge.   Final HORIZONTAL COORDINATE, its for the
	   //horiz. line that's the BOTTOM edge inclusively
	   //of the (closed) effective, non-cropped region.
	   //CROP_XF == x is the eq. of the BOTTOM edge
	   */

/* So ((horiz)x,(vert, top-to-bot)y) locates a pixel in the effective region
   IFF
   CROP_YI <= x <= CROP_YF AND CROP_XI <= y < CROP_XF
   (which is confusing to some of us)
*/

//scaleCD -- May 2026 we use this!
pixCoord UtoPmult; //Undefined, set in runtime after option processing.
pixCoord UtoP( pixCoord c ) { return UtoPmult*c; } //don't bother inlining or
// worse, compile-time configuration!  Today's optimizing compilers and
// out-of-ording, cached, pipelined CPU/ALUs make such hand tricks
// insignificant.  But, it's possible that separate cropping is
// more efficient than including that in exclusion zone functions,
// since cropping can REDUCE THE RANGE OF THE MAIN LOOP
// by \Theta(length*width).


//scaleCD
//dont use yet while still working on 1 vs 2 scaling in April 2026's
//4-way comparison project.
/** 
 *I'm using this ordering of variables since it's consistent
 * with the exclusion zone functions ((ezFun)(ii, jj)) below.
 */
bool DONT_USE_YET_CROP(  pixCoord vert, pixCoord horiz ) {
  return  ((UtoP(CROP_YI) <= horiz <= UtoP(CROP_YF)) && (UtoP(CROP_XI) <= vert < UtoP(CROP_XF)));
  //Whoppie! C/C++ accepts the mathematicians' syntax for ordered interval containment!
}

 
//will be set by init_CROPS_and_camera() at runtime
string camera;  //still custom, didn't integrate new camera_index
//for exclusion zone with a camera name.

//scaleCD -- April 2016 we use this
void init_CROPS_and_camera( void )
{
  //We have these preprocessor conditionals to
  //maintain regression as we develop support for
  //different cameras.

  //scaling?
#ifdef Custom
  CROP_YI = 0; //left edge
  CROP_XI = 0; //top edge
#else
  CROP_YI = 15; //scaleCD 15 and CROP.. is a UCoord
  CROP_XI = 10;
#endif
////scaleCD
// scale_factor == 1 ----Half-Resolution---Ucoord=Pcoord GOOD
// scale_factor == 2 --n-crop ----Full Resolution --Ucoord=(1/2) Pcoord GOOD!

  
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

 // cerr << "init_CROPS..CROP_XI=" << CROP_XI << " CROP_XF=" << CROP_XF << " CROP_YI=" << CROP_YI << " CROP_YF=" << CROP_YF << endl;
 //this is only good for the current task of comparing
 //well-working 3840x2080 movies scaled and decimated
 //to 1920x1040, for which the above pixel coordinates
 //are good, with our new version that works on
 //the full frame movie bitmaps.

 //end of init_CROPS_and_camera( void )
}

//scaling?
int Ret[2] = { 505, 85 }; //used ONLY by ezCamB1
int diffs[3] = { 0, 0, 0 };

//scaling?
pixCoord width, height;
uint32_t imgsizeb; //bytes, so =3*(num pix), for bmps (not yuv of course).
                   //Type used in .bmp spec.
                   //Must accomodate Megas, int16 no good.

static char * bmfilename( int f );

static uint8_t *BMP_A;
static uint8_t *BMP_B;
static int frameCnt = -1;

// overloaded versions for .bmp file

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
  readret = fread(info, sizeof(unsigned char), 54, f);

  // extract image height and width from header
  //scaling?
  width = *(pixCoord*)&info[18];
  height = *(pixCoord*)&info[22];
  fprintf(stderr, "%s gets %ux%u .bmp files.\n", progname, width, height);

  //scaling??
  // allocate three bytes per pixel
  imgsizeb = 3 * width * height;
  BMP_A = new uint8_t[imgsizeb];
  BMP_B = new uint8_t[imgsizeb];

  // read the rest of the data at once
  readret = fread(BMP_A, sizeof(uint8_t), imgsizeb, f);
  if( readret != imgsizeb){
    if( readret < 0 ) {
      error(1, errno, "Reading %s failed.", filename);
    }
      else {
	error(1, 0, "Only %ld of %u read, wrong bitmap fmt %s", readret, imgsizeb, filename);
      }
    }
  fclose(f);
  frameCnt = 1;
  
  for(int i = 0; i < imgsizeb; i += 3)
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
  readret = fread(dest, sizeof(unsigned char), imgsizeb, f);
  if( readret != imgsizeb){
    if( readret < 0 ) {
      error(1, errno, "Reading %s failed.", filename);
    }
    else {
	error(1, 0, "Only %ld of %u read, wrong bitmap fmt %s", readret, imgsizeb, filename);
      }
    }
  fclose(f);

  frameCnt++;
  
  for(int i = 0; i < imgsizeb; i += 3)
    {
      // flip the order of every 3 bytes
      uint8_t tmp = dest[i];
      dest[i] = dest[i+2]; dest[i+2] = tmp;
    }

  return ;
}


// overloaded versions for pipeline
int readFirstBMPToAandAllocB()
{
  int i;
  //FILE* fpin //

  unsigned char info[54];
  size_t readret;
  // read the 54-byte header
  readret = fread(info, 54, 1, fpin);
  if(vb) {
    stdinfreads++;
    fprintf(stderr, "readFirstBMP: %dth read of header. readret=%ld.\n", stdinfreads, readret);
  }
  
  // extract image height and width from header
  width = *(int*)&info[18];
  height = *(int*)&info[22];
  if(vb) fprintf(stderr, "%s gets stdin stream of %ux%u .bmps\n", progname, width, height);
  //cerr << width << " " << height << endl;

  //scaling??
  // allocate three bytes per pixel
  imgsizeb = 3 * width * height;
  BMP_A = new unsigned char[imgsizeb];
  BMP_B = new unsigned char[imgsizeb];

  // read the rest of the data at once
  // MYSTERY WE GET EOF HERE!!
  readret = fread(BMP_A, imgsizeb, 1, fpin);
  if(vb) {
    stdinfreads++;
    fprintf(stderr, "readFirstBMP stream %dth read pixels. readret=%ld. errno=%d feof=%d\n", stdinfreads, readret, errno, feof(fpin));
  }
  if( 1 != readret ){
    if( readret < 0 ) {
      error(1, errno, "First frame: Reading stdin failed.");
    }
      else {
	error(1, errno, "First frame: When reading image data, only %ld of %u read.", readret, imgsizeb);
      }
    }
  // don't close fpin!
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
  //FILE* fpin = stdin; //initialized in global decl.
  //fpin is now global
  //if( !fpin ) {
  //  error(1, errno, "Opening %dth frame %s failed.", frameCnt+1, filename);
  //}
  unsigned char info[54];
  size_t readret;

  if(vb) fprintf(stderr, "readSubseq stream for header after %dth read\n", stdinfreads);
  
  // read the 54-byte header or detect EOF! feof != 0 means EOF.
  readret = fread(info, 54, 1, fpin);
  if( readret < 0 ) {
    error(1, errno, "Subsequent stream bmps: fread ret %ld header from stdin failed, not EOF?", readret);
  }

  if( readret != 1){
    if(vb) fprintf(stderr, "readSubseq bmp stream readret!=1, = %ld\n", readret);
    if( feof(fpin) != 0 ) {
      //There are no more bitmaps to process.
      // Should we clear the error?  I think so
      // since I hope we can process multiple movies
      // this way, which will entail replacing the
      // ffmpeg movie->bitmaps process and our pipe
      // from it.
      if(vb) fprintf(stderr, "input fp is not at feof, we will clear.\n");
      clearerr(fpin);
      return 1;
    }
  }
  //Good case, go on.
  
  // extract image height and width from header
  // check consistency of size?? Paranoia hits.
  //scaling?
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
  readret = fread(dest, imgsizeb, 1, fpin);
  if( 1 != readret ){
    if( readret < 0 ) {
      error(1, errno, "Subsequent stream pixel data: Reading a whole bitmaps data failed.");
    }
    else {
	error(1, errno, "Subsequent stream pixel data: Only %ld of %u read, wrong bitmap fmt", readret, imgsizeb);
      }
  }

  //still good
  if(vb) {
    stdinfreads++; fprintf(stderr,"%dth stream read of pixel data \n", stdinfreads);
  }

  frameCnt++;
  
  for(int i = 0; i < imgsizeb; i += 3)
    {
      // flip the order of every 3 bytes
      uint8_t tmp = dest[i];
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
// watch out! ii is the vertical coord, jj is the horiz coord
// Top is 0, bottom is height-1

static bool ezNone( pixCoord ii, pixCoord jj ) {
  return false;
}


//scaleCD Yes.. ii, jj are Pcoord and 73, 1568 are Ucoord. GOOD
static bool ezDroneCalib1( pixCoord ii, pixCoord jj ) {
  return (ii < UtoP(73) ) && (jj > UtoP(1568) );
  //         ^                ^
  //         |                |
  //         |                |-- Right of a vertical line
  //         |                    near the left edge.
  //---------^ Above line 73 pix down from top
  //Exclude the upper right date-time exhibit
  //These coords are wrt the 1920x1040 frame, which is 1/2 the original
  //DroneCalib1.mp4 (and DroneShort1.mp4) movies.
  //1920x1040 is in Ucoord for this series of movies.
}

//scaleCD
//scaling? all ez* functions!
//UtoP( consts ) below NOT DONE YET

///////////////////////// MUST BE MODIFIED IF PROCESSING CHANGES ///////////////
////////////////THE RESOLUTION WHEN THESE CAMERA VIDEOS ARE EXTRACTED FOR US///
///////////////////////////////////////////////////////////////////////////////

//scaleCD--yes, pixCoord==Pcoord are what we should use here,
//given these are called with loop indices, which are Pcoords.
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

// for camera databae
// We eventually will NOT have these error-prone separate intializers
// Also, camera will be selected by name, NOT -camera-index !
struct camera {
  const char *name;
  bool (*ezFun)(pixCoord, pixCoord);
} cameras[] = {
  { "None",  ezNone },
  { "DroneCalib1", ezDroneCalib1 },
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
  { ezNone, ezDroneCalib1, ezCamA1, ezCamA2, ezCamA3, ezCamA4,
    ezCamB1, ezCamB2, ezCamB3, ezCamB4 };

int camera_index = 0;

void optionprocess()
{
  if( cropargs != 0 ) {
    if (cropargs != 4) {
      error(1, 0, "CROP_[X,Y][I,F] options: Must give none or all 4!");
    }
    // cropargs == 4
    if( no_crop ) {
      cerr << progname << "WARNING: both CROP_[X,Y][I,F] option and no-crop options were given. We will not crop." << endl;
      cropargs = 0;
    }
    else {
      cerr << progname << " CROP values we be set from args : " << endl;
      cerr << "Top-Bot: CROP_XI=" << argCROP_XI << " CROPXF=" << argCROP_XF << endl;
      cerr << "Lft-Rht: CROP_YI=" << argCROP_YI << " CROPYF=" << argCROP_YF << endl;
      /* We should use these values, from the argv, for cropping no matter what */
      /*  pixCoord argCROP_XI, argCROP_XF, argCROP_YI, argCROP_YF; */
      /* Maybe do more here. */
    }
  }
  //We exited or we are done argCROP options.  Other option processing can go here.
}

int main( int argc, char** argv ) {
  progname = argv[0];
  cerr << progname << "  called!" << endl;
  get_our_options( & argc, & argv);
  // --bitmaps-dir and --CamSett-file are really options.
  // if not supplied, the original defaults that work when
  // these files are in cwd are used.
  // --pipeline is a option, default is to read .bmp files.

  optionprocess();
  //Scaling
  if((Pscale > Mscale) || Pscale*(Mscale/Pscale) != Mscale)
    {
      error(1,0,"Not Supported yet: Bigger Pscale(=%u) or Pscale doesnt divide Mscale(=%u).\n",
	    Pscale, Mscale);
    }
  // To develop this while comparing results from the original testing with DroneShort1,
  // we pretend the movie to pixel scale is 1-1 because the parameters were in terms
  // of the halved movie scale.  So we use --movie-scale 1 --pixproc-scale 1
  // for the original, and for full resolution, we use --movie-scale 2 -pixproc-scale 1
  // all with --user-scale 1 set by default.
  // (We no longer pretend anything.)
  UtoPmult = Pscale/Uscale;
  //For now, we don't use or check Uscale, nor do other scaling.
  if( UtoPmult != 1 )
    fprintf(stderr,"Compile Time and CamSett crop constants will be *%u before comparison with a pix coord.\n",
	    UtoPmult);

  //Might be set by get_our_options.
  //Must set UtoPmult first.
  init_CROPS_and_camera( ); //global above,


  
  inExclusionZone = cameras[camera_index].ezFun;
  /* make it point to the right function */

  cerr << "camera_index setting inExclusionZone is " << camera_index << endl;

  if(argc < 2) {
    error(1, 0,
	  "Phase1aPipeOpt [--pipeline] [other opts] startnumber howmanyframes [Cloud Cover, not used].");
  }

  unsigned long start = (unsigned long)atof(argv[1]);
  unsigned long end = start + (unsigned long)atof(argv[2]);
  //arg[3] is unused!
  if( argc >= 4 ) {
    bool CC = atoi(argv[3]); //CC is unused??
  }  
  char line[80]; double temp; vector<double> CamSett;

  //time awaits for more reliable to use key based CamSett file interpretation
  FILE *file = fopen(CamSett_file,"r");
  if( !file ) {
    error(1, errno, "CamSett.txt file missing from CWDir.");
  }

  size_t ret;
  for ( unsigned short k = 0; k < 20; ++k ) {
    ret = fscanf ( file, "%s %lf", line, &temp );
    CamSett.push_back(temp);
  }
  ret = fscanf ( file, "%s", line ); fclose(file);
  unsigned short MinThr = (unsigned short)CamSett[19], SubThr = (unsigned short)CamSett[10];
  fprintf(stderr,"%s MinThr=%u SubThr=%u\n", progname, MinThr, SubThr);
  //We don't write cameraName= ...  at the end of CamSett.txt anymore.

#ifdef Custom
  CROP_XI = (unsigned int)CamSett[15];
  CROP_XF = (unsigned int)CamSett[16];
  CROP_YI = (unsigned int)CamSett[17];
  CROP_YF = (unsigned int)CamSett[18];
#endif

  unsigned long k = start;

  //attn C++ newbies: We took advantage of C++ function overloading.
  if(pipeline) {
    readFirstBMPToAandAllocB( );
  }
  else {
    readFirstBMPToAandAllocB( bmfilename(k+1));
  }
  //Those allocated these two bmp buffers.
  unsigned char* dataOld = BMP_B; //variables used for
  unsigned char* dataNew = BMP_A; //double buffering

  cerr << progname <<
    "CROP param from settings, user (not pix/proc) coords, [[I<=,,<F)) :" << end;
  cerr <<  "Top/Bot  CROP_XI=" << CROP_XI << " CROP_XF=" << CROP_XF << endl;
  cerr <<  "Lft/Rht  CROP_YI=" << CROP_YI << " CROP_YF=" << CROP_YF << endl;

  //Cropping merely reduces the main inner loop ranges.
  //The inner loop is the most costly step after frame extraction when you
  //count bringing the frames into memory, but
  //it can be (fairly) easily parallelized with threads.
  
  int i_loop_from, i_loop_lt, j_loop_from, j_loop_lt;
    
  if(no_crop) {
    i_loop_from = 0;
    i_loop_lt   = height;
    j_loop_from = 0;
    j_loop_lt   = width;
    cerr <<
      "Option no-crop has been activated: CROP_var cropping will not be done." << endl;
  }
  else {
    if(cropargs) {
      CROP_XI = argCROP_XI;
      CROP_XF = argCROP_XF;
      CROP_YI = argCROP_YI;
      CROP_YF = argCROP_YF;
      cerr << "CamSett CROPs overruled by crop-args options" << endl;
    }
    i_loop_from = height - UtoP(CROP_XF);  //so, for now CROPS should certainly
    i_loop_lt   = height - 1 - UtoP(CROP_XI);    //be in the Pcoord system.
    j_loop_from = UtoP(CROP_YI);
    j_loop_lt   = UtoP(CROP_YF);
  }

  //When all is said and one, we insist upon soundness of CROP parameters.
  if( (!( 0 <= i_loop_from < i_loop_lt <= height )) ||
      (!( 0 <= j_loop_from < j_loop_lt <= width )) ) {
    error(1,0,"%s CROP_XYIFs (from CamSett and/or options) determines a processing region\n"
	  "that is empty or exceeds the pixel image.\n"
	  "Image(WxH)range=[0,%d)X(0,%d)\nCrop range=[%d,%d)X[%d,%d).", progname,
	  height, width, j_loop_from, j_loop_lt, i_loop_from, i_loop_lt);
  }
  
  //Loop over our input of frames after the first. 
  //We swap the double buffers before reading the next frame.
  //We can terminate the loop either by specifying a range of frame numbers (k<end),
  //or, only in pipeline processing (for now), detecting EOF after the last frame (break;).

  for ( k = start+1; k < end; ++k ) {
    //swap
    uint8_t * tem = dataOld; dataOld = dataNew; dataNew = tem; 
    //loop end test, 2nd case.
    if(pipeline) {
      if( 0 != readSubsequentBMP (dataNew) )
	{ //no more bitmaps
	  break; //out of for( k = ... )
	}
    }
    else {
         readSubsequentBMP ( bmfilename(k+1), dataNew );
    }

    int maximum[3] = {-1,-1,-1}; int minimum[3] = {256,256,256};
    int maxLoc[3][2] = {0}; int minLoc[3][2] = {0};
    int NumPixAbvThr[3][2] = {0};
    int NumPixAbvSubThrSum = 0;
    
    int CloudCover = 100;
    
    //pixel process loop:
    for ( int i = (i_loop_from); i < (i_loop_lt); ++i ) {
      for ( int j = (j_loop_from); j < (j_loop_lt); ++j ) {
	
	int rgbColorNew[3], rgbColorOld[3];
	
	rgbColorOld[0] = (int)dataOld[3 * (i * width + j) + 0]; //Tiny Old*=New*
	rgbColorOld[1] = (int)dataOld[3 * (i * width + j) + 1]; //optimization
	rgbColorOld[2] = (int)dataOld[3 * (i * width + j) + 2]; //not done for simplicity.
	
	rgbColorNew[0] = (int)dataNew[3 * (i * width + j) + 0];
        rgbColorNew[1] = (int)dataNew[3 * (i * width + j) + 1];
        rgbColorNew[2] = (int)dataNew[3 * (i * width + j) + 2];
	
	diffs[0] = rgbColorNew[0]-rgbColorOld[0];
	diffs[1] = rgbColorNew[1]-rgbColorOld[1];
	diffs[2] = rgbColorNew[2]-rgbColorOld[2];
	int jj = j;
	int ii = height - 1 - i;//ii is increasing Top to Bottom, non-Microsoft y-axis:vdir order.

	//exclusion zone and cropping coordinates are in the incr. Top to Bottom order.
	//(For our direct uses of i, j, it doesn't matter.)

	//old code if( k==start ) removed, init k = start+1
	if ( !(*inExclusionZone)(ii, jj) ) {

          //////////////////////////////////////////////////////////////////////////////////////////////
	  /////////////////////////////////////////FLIR PHASE1A Calculation/////////////////////////////
          //////////////////////////////////////////////////////////////////////////////////////////////

	  
	  if ( diffs[0] > maximum[0] ) {
	    maximum[0] = diffs[0]; maxLoc[0][0] = ii; maxLoc[0][1] = jj;
	    if ( maximum[0] > MinThr ) ++NumPixAbvThr[0][0];
	    if ( maximum[0] > SubThr ) ++NumPixAbvSubThrSum;
	  }
	  if ( diffs[1] > maximum[1] ) {
	    maximum[1] = diffs[1]; maxLoc[1][0] = ii; maxLoc[1][1] = jj;
	    if ( maximum[1] > MinThr ) ++NumPixAbvThr[1][0];
	    if ( maximum[1] > SubThr ) ++NumPixAbvSubThrSum;
	  }
	  if ( diffs[2] > maximum[2] ) {
	    maximum[2] = diffs[2]; maxLoc[2][0] = ii; maxLoc[2][1] = jj;
	    if ( maximum[2] > MinThr ) ++NumPixAbvThr[2][0];
	    if ( maximum[2] > SubThr ) ++NumPixAbvSubThrSum;
	  }
	  if ( diffs[0] < minimum[0] ) {
	    minimum[0] = diffs[0]; minLoc[0][0] = ii; minLoc[0][1] = jj;
	    if ( minimum[0] < -MinThr ) ++NumPixAbvThr[0][1];
	    if ( minimum[0] < -SubThr ) ++NumPixAbvSubThrSum;
	  }
	  if ( diffs[1] < minimum[1] ) {
	    minimum[1] = diffs[1]; minLoc[1][0] = ii; minLoc[1][1] = jj;
	    if ( minimum[1] < -MinThr ) ++NumPixAbvThr[1][1];
	    if ( minimum[1] <- SubThr ) ++NumPixAbvSubThrSum;
	  }
	  if ( diffs[2] < minimum[2] ) {
	    minimum[2] = diffs[2]; minLoc[2][0] = ii; minLoc[2][1] = jj;
	    if ( minimum[2] < -MinThr ) ++NumPixAbvThr[2][1];
	    if ( minimum[2] < -SubThr ) ++NumPixAbvSubThrSum; }
	  
	}
	
      } /* end pixel y loop */
    } /* end pixel x loop */
    
    //Output Vertical Pixel coodinates are REVERSED compared to input images.
    //The input, being a .bmp (even current in pipelined version), uses
    //Microsoft's bottom-is-zero convention.  

    //scaling? OUTPUT is in processing/pixel coords.  //scaleCD yes, we are outputting Pcoords

    const char oldformat[] = "%lu\t\t%i  %i %i\t%i  %i %i\t%i  %i %i\t\t%i  %i %i\t%i  %i %i\t%i  %i %i\t\t%i %i %i\t%i %i %i\t\t%d\n";

    const char newformat[] ="%-8lu %4i  %4i %4i   %4i  %4i %4i   %4i %4i %4i     %4i  %4i %4i   %4i  %4i %4i   %4i %4i %4i   "
    //                           k   1   2   3     4    5   6     7    8   9      10   11  12    13   14  15    16  17  18
                            "%2i %2i %2i   %2i %2i %2i   %i\n";
    //                        19  20  21    22  23  24   25 
    printf(newformat,
	      k,
	   maximum[0],maxLoc[0][1],maxLoc[0][0],  // 1  2  3
	   maximum[1],maxLoc[1][1],maxLoc[1][0],  // 4  5  6
	   maximum[2],maxLoc[2][1],maxLoc[2][0],  // 7  8  9
	   minimum[0],minLoc[0][1],minLoc[0][0],  //10 11 12
	   minimum[1],minLoc[1][1],minLoc[1][0],  //13 14 15
	   minimum[2],minLoc[2][1],minLoc[2][0],  //16 17 18
	   NumPixAbvThr[0][0],NumPixAbvThr[1][0],NumPixAbvThr[2][0],  //19 20 21
	   NumPixAbvThr[0][1],NumPixAbvThr[1][1],NumPixAbvThr[2][1],  //22 23 24
	   NumPixAbvSubThrSum); //25

    
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
      {"bitmaps-dir",     required_argument, 0,  0 }, //0
      {"CamSett-file",    required_argument, 0,  0 }, //1
      {"movie-scale",   required_argument, 0,  0 },   //2
      {"pixproc-scale", required_argument, 0, 0 },    //3
      {"user-scale",    required_argument, 0, 0 },    //4
      {"no-crop",  no_argument, &no_crop, 1 },        //5
      {"camera-index", required_argument, 0, 0},      //6
      {"pipeline", no_argument, &pipeline, 1},        //7 top of file

      {"verbose", no_argument, 0, 0},               //8 ignored for regression devel.
      
      //These 5 override CROP settings from CamSett.
      //This is done when all four CROPs are given and no-crop is not set.
      {"crop-args", no_argument, 0, 0},               //9  //ignore for now! 
      {"CROP_XI", required_argument, 0, 0},           //10 These 4 are used only 
      {"CROP_XF", required_argument, 0, 0},           //11 if --crop-args is set.
      {"CROP_YI", required_argument, 0, 0},           //12
      {"CROP_YF", required_argument, 0, 0},           //13

      {0,         0,                 0,  0 }
    };
    c = getopt_long( *argc, *argv, "",
		    long_options, &option_index);
    if (c == -1)
      break;

    switch (c) {
    case 0:
      switch (option_index) {
      case 0: bitmaps_dir  = optarg; /*cstring*/    break;
      case 1: CamSett_file = optarg; /*cstring*/    break;
	//Below will be processed by main.
      case 2: Mscale = atoi(optarg); /*uint*/ break;
      case 3: Pscale = atoi(optarg); /*uint*/ break;
      case 4: Uscale = atoi(optarg); /*uint*/ break;
      case 5: /*no_crop set by getopt_long action on long_options[] */
	break;
      case 6: camera_index = atoi(optarg); /*cstring*/ break;
	//ignore case 9 crop-args
      case 8: fprintf(stderr, "%s Ignored Option --verbose\n", (*argv)[0]); break;
      case 10: cropargs++; argCROP_XI = atoi(optarg); break;
      case 11: cropargs++; argCROP_XF = atoi(optarg); break;
      case 12: cropargs++; argCROP_YI = atoi(optarg); break;
      case 13: cropargs++; argCROP_YF = atoi(optarg); break;

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
  return 0; //_GNU_SOURCE getopt_long exits on illegal option spelling or missing req. arg.
}
