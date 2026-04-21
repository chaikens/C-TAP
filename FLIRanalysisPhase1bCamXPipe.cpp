
/**

Writes result to put in .out file to stdout.

Information report we should save, and errors, are written to stderr.

Literature ref: 
Section 3.2.1 on image analysis for UAP research starting on page 11 of https://arxiv.org/pdf/2312.00558

  FLIRanalysisPhase1bCamXPipe.cpp, compiled to Phase1aPipe

  $ Phase1BPipe [--verbose] input_filename NumFrames level

 number_of_frames_to_process \
    < (pipe or file containing a concatenation of bitmaps).

  It reads parameters from CamSett.txt from its current working dir.
  It writes CamSett.tmp which then replaces CamSett.txt.

  It writes to stdout a one line report for each "golden event"
  Pixel coodinates (whose meaning of course depends on image resolution)
  are (apparently) not used in the calculation, but they are passed into
  reports.


  It writes informative messages to stderr. (Should we retain those better?)



  [not yet] --verbose does writing to stderr (cerr).

   Phase1bPipe [not yet --verbose] StartFrameNumber NumberOfFrames CloudCover(not used)



   (still looking) There are no other external non-standard dependencies or effects.

Explanation of input, saved in moviename.in file:

- MV.int text file result of analyzing pixel value differences at common coordinate.
  
  example::

    1 34 1908 802 34 1908 802 34 1908 802 -30 1247 858 -30
    1247 858 -30 1247 858 4 4 4 7 7 7 21

    2 38 1800 872 38 1800 872 38 1800 872 -28 1682 790 -28 1682 790 -28
    1682 790 8 8 8 4 4 4 24

    3 31 43 522 31 43 522 31 43 522 -32 1005 370 -32 1005 370 -32 1005 370
    6 6 6 4 4 4 18

  shown grouped and explained::

    +---------------+--------------------------------------------------+
    | 4             | difference frame number from Frame_i-Frame_(i-1) |
    +---------------+--------------------------------------------------+
    | 34 721 676    | max pos differences                              |
    | 34 721 676    | for each color in rgb order,                     |
    | 34 721 676    |   (diff value) (x-coord) (y-coord)               |
    +---------------+--------------------------------------------------+
    | -34 1851 744  | min neg differences                              |
    | -34 1851 744  |    ditto                                         |
    | -34 1851 744  |                                                  |
    +---------------+--------------------------------------------------+
    | 4 4 4         | numbers of times a pix diff breaks a threshold   |
    | 5 5 5         | first line positives, 2nd negatives (rgb order)  |
    +------------------------------------------------------------------+
    | 21            | please look and report, important number         |
    +------------------------------------------------------------------+

Explanation of output, going into moviename.out file, to
be visualized by drawing a circle on a frame  (from Matt's conversation, March 15, 2026:

  Each line corresponds to a one-frame pair event, as determined by the software.
  
  -   cluster number  (may be repeated, clusters of events are numbered successively)
  -   frame number  (of one event)
  - signed pixel diff with max abs value
  - x, y coords in the frame of the pixel with diffs max abs value
  - naive bayisan classifier score range 0 to  1 corresponds to a 
    prediction quality ranging from garbage to a real thing.

*/

#include <fstream>
#include <cstdio>
#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <cmath>
#include <math.h>
#include <vector>
#include <cassert>

using namespace std;

vector<bool> SignalTruth;
double prob[555000];

int main ( int argc, char** argv ) {
  
  double data[26]; vector<double> store[26]; vector<short> extrema;
  unsigned long i = 0;
  signed short j = 0;
  float AbsAvg, AbsStdDev, FracYes;
  double GlobPixMean[7] = {0.,0.,0.,0.,0.,0.,0.};
  
  char line[80];
  double temp;
  vector<double> CamSett;
  int RewFram, ForFram, FramBefNew, SubThr;
  FILE *CamSetfile = fopen("CamSett.txt","r");
  FILE *NewCamSetfile = fopen("CamSett.tmp","w");
  for ( j = 0; j < 20; ++j ) {
    fscanf ( CamSetfile, "%s %lf", line, &temp );
    CamSett.push_back(temp);
  }
  char camName[3];
  fscanf ( CamSetfile, "%s %s", line, camName );
  string camera(camName);
  rewind(CamSetfile);
  for ( j = 0; j < 20; ++j ) {
    fgets(line, sizeof(line), CamSetfile);
    fputs(line, NewCamSetfile);
  }
  fclose(CamSetfile);
  fclose(NewCamSetfile);
  system("mv CamSett.tmp CamSett.txt");
  if ( camera == "Custom" ) {
    SubThr = (int)CamSett[10];
    RewFram= (int)CamSett[11];
    ForFram =(int)CamSett[12];
    FramBefNew = (int)CamSett[13];
    FracYes = (float)CamSett[14];
  }
  else {
    RewFram = 100;
    ForFram = 320;
    FramBefNew = 320;
    SubThr = 20;
    FracYes = 0.07;
  }
  
  int smallestThr, biggestThr, smallestPix, biggestPix;
  if ( camera == "B1" || camera == "A3" ) { // "bad" camera
    smallestThr = 34;
    biggestThr = 54;
    smallestPix = 3;
    biggestPix = 30;
  }
  else if ( camera == "B3" || camera == "B4" ) { // "medium"-quality cam
    smallestThr = 0;
    biggestThr = 254;
    smallestPix = 0;
    biggestPix = 69; //set to min allowed for catching object starting at frame 41935 in camB3 (14_00-59-40). Line 197 makes this hist
  }
  else if ( camera == "A1" || camera == "A2" || camera == "A4" || camera == "B2" ) { // "good" camera
    smallestThr = 0;
    biggestThr = 254;
    smallestPix	= 0;
    biggestPix = 300; //200 is min allowed to catch object at 09:20:48-50am in CamA4 (July 15)
  }
  else {
    smallestThr = (int)CamSett[0];
    biggestThr = (int)CamSett[1];
    smallestPix = (int)CamSett[2];
    biggestPix = (int)CamSett[3];
  }
  
  ifstream ifp(argv[1]);
  if(!ifp.is_open()) {
    //bad ugly C++ this failed to detect bad file. I hacked reading when loop index i==0
    cerr << "Phase1b data input file " << argv[1] << "failed" << endl;
    return 1;
  }
  long NumFrames = atol(argv[2]); //maybe use as an upper limit for num diffs to process
                                  //when it is used as a filter.
  double level = atof(argv[3]);
  if ( level >= 0.979 && camera != "B1" ) {
    if ( camera == "B2" ) level = 0.999999999999; // the minimum allowed that doesn't make B2 collect too many events (16_21-57-25)
    else if ( camera == "B3" ) level = 0.9999999999;
    else if ( camera == "A3" ) level = 0.9999999; // 15_19-57-27 8:51:30-53pm (boat?)
    else if ( camera == "Custom" ) { ; }
    else level = 0.99999999999; // 11 9's
  }
  
  long NumFramesRead = 0;
  
  for ( i = 0; i < NumFrames; ++i ) {
    
    if (ifp >> data[0] >> data[1] >> data[2] >> data[3] >> data[4] >> data[5]
	>> data[6] >> data[7] >> data[8] >> data[9] >> data[10] >> data[11]
	>> data[12] >> data[13] >> data[14] >> data[15] >> data[16] >> data[17]
	>> data[18] >> data[19] >> data[20] >> data[21] >> data[22] >> data[23]
	>> data[24] >> data[25] )
      { //we read a proper line of data

	NumFramesRead++;
	
	GlobPixMean[0] += data[1];
	GlobPixMean[1] += data[4];
	GlobPixMean[2] += data[7];
	GlobPixMean[3] -= data[10];
	GlobPixMean[4] -= data[13];
	GlobPixMean[5] -= data[16];
	GlobPixMean[6] += data[25];
    
	for ( j = 0; j < 26; ++j ) {
	  store[j].push_back(data[j]);
	}
      }
    else
      { //read of next or first line failed.
	if( i == 0 )
	  { //can't even read the first line!
	    cerr << "Failure to read first line from " << argv[1] << endl;
	    cerr << "Bad filename or wrong numberical format." << endl;
	    return 1;
	  }
	else
	  {
	    cerr << argv[1] << " file reading line limit reached NumFrames=" <<
	      NumFrames << "  NumFramesRead=" << NumFramesRead << endl;
	    NumFrames = NumFramesRead;
	    break; //number of lines exceeds NumFrames
	  }
      }
  }
  ifp.close();
  assert(NumFrames == NumFramesRead);
  //  NumFrames = NumFramesRead;
  
  for ( j = 0; j < 7; ++j )
    { GlobPixMean[j] /= double(NumFrames); }
  double OverallAverage= (GlobPixMean[0]+GlobPixMean[1]+GlobPixMean[2]+GlobPixMean[3]+GlobPixMean[4]+GlobPixMean[5]) / 6.;
  double OverallStdDev = pow(OverallAverage-GlobPixMean[0],2.)+
                         pow(OverallAverage-GlobPixMean[1],2.)+
                         pow(OverallAverage-GlobPixMean[2],2.)+
                         pow(OverallAverage-GlobPixMean[3],2.)+
                         pow(OverallAverage-GlobPixMean[4],2.)+
                         pow(OverallAverage-GlobPixMean[5],2.);
  OverallStdDev /= 5.; OverallStdDev = sqrt(OverallStdDev);
  fprintf(stderr,"Grey(8-bit) pixel diff mu and sigma of %.1f +/- %.2f (units of 0-255)\n",OverallAverage,OverallStdDev);
  int MinThr = std::max(int(floor(OverallAverage+OverallStdDev*floor(OverallStdDev)-0.5)),smallestThr); //33 for over-fit to initial test
  if ( MinThr > 43 && camera == "B1" ) MinThr = 43;
  int MaxThr = biggestThr;
  fprintf(stderr,"So, setting a minimum threshold of %.1f SIGMA = %i (so-called SUB threshold of %d)\n",floor(OverallStdDev),MinThr,SubThr);
  fprintf(stderr,"And setting a maximum threshold of %i\n",MaxThr);
  double GlobPixSigma = 0.0;
  for ( i = 0; i < NumFrames; ++i ) {
    GlobPixSigma += pow(GlobPixMean[6]-store[25][i],2.);
  }
  GlobPixSigma /= (double(NumFrames)-1.);
  GlobPixSigma = sqrt(GlobPixSigma);
  fprintf(stderr,"The number of pixels above %d units is %.2f +/- %.2f\n",SubThr,GlobPixMean[6],GlobPixSigma);
  int MinPix = std::max(smallestPix,int(ceil(GlobPixMean[6]+1.)));
  int MaxPix = biggestPix;
  while ( MaxPix <= MinPix ) {
    if ( camera == "B1" )
      return 1;
    else
      MaxPix += 5;
  }
  fprintf(stderr,"Minimum number of pixels allowed to be above the sub-threshold of %d is %d\n",SubThr,MinPix);
  fprintf(stderr,"Maximum number of pixels allowed to be above the sub-threshold of %d is %d\n",SubThr,MaxPix);
  
  for ( i = 0; i < (NumFrames-1); ++i ) {
    
    unsigned long frameNum = (unsigned long)store[0][i];
    unsigned short maxR = (unsigned short)store[1][i];
    unsigned int xR = (unsigned int)store[2][i];
    unsigned int yR = (unsigned int)store[3][i];
    unsigned short maxG = (unsigned short)store[4][i];
    unsigned int xG = (unsigned int)store[5][i];
    unsigned int yG = (unsigned int)store[6][i];
    unsigned short maxB = (unsigned short)store[7][i];
    unsigned int xB = (unsigned int)store[8][i];
    unsigned int yB = (unsigned int)store[9][i];
    unsigned short maxr = (unsigned short)(-store[10][i]);
    unsigned int xr = (unsigned int)store[11][i];
    unsigned int yr = (unsigned int)store[12][i];
    unsigned short maxg = (unsigned short)(-store[13][i]);
    unsigned int xg = (unsigned int)store[14][i];
    unsigned int yg = (unsigned int)store[15][i];
    unsigned short maxb = (unsigned short)(-store[16][i]);
    unsigned int xb = (unsigned int)store[17][i];
    unsigned int yb = (unsigned int)store[18][i];
    unsigned short NumPixAbvThrR = (unsigned short)store[19][i];
    unsigned short NumPixAbvThrG = (unsigned short)store[20][i];
    unsigned short NumPixAbvThrB = (unsigned short)store[21][i];
    unsigned short NumPixAbvThrr = (unsigned short)store[22][i];
    unsigned short NumPixAbvThrg = (unsigned short)store[23][i];
    unsigned short NumPixAbvThrb = (unsigned short)store[24][i];
    
    unsigned int NumPixAbvThrSum = NumPixAbvThrR + NumPixAbvThrG + NumPixAbvThrB + NumPixAbvThrr + NumPixAbvThrg + NumPixAbvThrb;
    unsigned int NumPixAbvSubThrSum = (unsigned int)store[25][i];
    
    extrema.clear();
    extrema.push_back(maxR);
    extrema.push_back(maxG);
    extrema.push_back(maxB);
    extrema.push_back(maxr);
    extrema.push_back(maxg);
    extrema.push_back(maxb);
    std::sort( extrema.begin(), extrema.end() );
    unsigned short AbsMax =abs(extrema.back());
    
    AbsAvg = ( maxR + maxG + maxB + maxr + maxg + maxb ) / 6.;
    AbsStdDev = ( maxR - AbsAvg ) * ( maxR - AbsAvg ) + ( maxG - AbsAvg ) * ( maxG - AbsAvg ) + ( maxB - AbsAvg ) * ( maxB - AbsAvg ) +
      ( maxr - AbsAvg ) * ( maxr - AbsAvg ) + ( maxg - AbsAvg ) * ( maxg - AbsAvg ) + ( maxb - AbsAvg ) * ( maxb - AbsAvg );
    AbsStdDev = sqrt ( AbsStdDev / 5. );
    vector<double> SkewGauss(4); // "classic" values for the next line: ampl .633, mu 1.97, sig 1.89, skew 2.5
    if ( camera == "Custom" ) {
      SkewGauss[0] = CamSett[4];
      SkewGauss[1] = CamSett[5];
      SkewGauss[2] = CamSett[6];
      SkewGauss[3] = CamSett[7];
    }
    else {
      SkewGauss[0] = 0.673;
      SkewGauss[1] = 2.;
      SkewGauss[2] = 2.;
      SkewGauss[3] = 2.0;
    }
    prob[i] = 1. - SkewGauss[0]*exp(-0.5*(AbsStdDev-SkewGauss[1])*(AbsStdDev-SkewGauss[1])/(SkewGauss[2]*SkewGauss[2]))*(1.+erf(SkewGauss[3]*(AbsStdDev-SkewGauss[1])/(SkewGauss[2]*sqrt(2.))));
    
    if ( (prob[i] > level && (AbsMax > MinThr && AbsMax < MaxThr) && NumPixAbvSubThrSum > MinPix && NumPixAbvSubThrSum < MaxPix) ||
	 (NumPixAbvThrSum > 36 && NumPixAbvThrSum < 44 && camera == "B1") || (NumPixAbvThrSum > 100 && (camera == "B3" || camera == "B4")) ||
	 (camera == "Custom" && NumPixAbvThrSum > CamSett[8] && NumPixAbvThrSum < CamSett[9]) ) {
      SignalTruth.push_back(true);
    }
    else {
      SignalTruth.push_back(false);
    }
    
  }
  
  double previous[4] = {1.,256.,0.,0.}; unsigned short int iEvtN = 0; // the global event number
  unsigned short FrameNum[2] = { 0, 0 }; if ( camera != "Custom" ) SignalTruth.erase(SignalTruth.begin());
  
  for ( i = RewFram; i < (NumFrames-ForFram-2); ++i ) {
    
    bool GoldenEvent = false; unsigned int NumBools = 0;
    if ( camera == "Custom" ) {
      if ( SignalTruth[i] ) GoldenEvent = true;
    }
    else {
      if ( SignalTruth[i-1] ) GoldenEvent = true;
    }
    for ( j = -RewFram; j < ForFram; ++j ) {
      if ( SignalTruth[i+j] ) ++NumBools; //used with "FracYes" below
    }
    
    if ( GoldenEvent || float(NumBools) / float(ForFram+RewFram) > FracYes )
      {
	unsigned short maxR = (unsigned short)store[1][i];
	unsigned short maxG = (unsigned short)store[4][i];
	unsigned short maxB = (unsigned short)store[7][i];
	unsigned short maxr = (unsigned short)(-store[10][i]);
	unsigned short maxg = (unsigned short)(-store[13][i]);
	unsigned short maxb = (unsigned short)(-store[16][i]);
	extrema.clear();
	extrema.push_back(maxR); extrema.push_back(maxG); extrema.push_back(maxB); extrema.push_back(maxr); extrema.push_back(maxg); extrema.push_back(maxb);
	std::sort(extrema.begin(),extrema.end()); unsigned short AbsMax = abs(extrema.back());
	extrema.clear();
	if ( AbsMax == maxR ) { extrema.push_back( maxR); extrema.push_back(store[2][i]); extrema.push_back(store[3][i]); }
	else if ( AbsMax == maxG ) { extrema.push_back( maxG); extrema.push_back(store[5][i]); extrema.push_back(store[6][i]); }
	else if ( AbsMax == maxB ) { extrema.push_back( maxB); extrema.push_back(store[8][i]); extrema.push_back(store[9][i]); }
	else if ( AbsMax == maxr ) { extrema.push_back(-maxr); extrema.push_back(store[11][i]); extrema.push_back(store[12][i]); }
	else if ( AbsMax == maxg ) { extrema.push_back(-maxg); extrema.push_back(store[14][i]); extrema.push_back(store[15][i]); }
	else { extrema.push_back(-maxb); extrema.push_back(store[17][i]); extrema.push_back(store[18][i]); }
	
	FrameNum[1] = store[0][i];
	
	if ( (FrameNum[1]-FrameNum[0]) > FramBefNew && FrameNum[0] != 0 ) ++iEvtN;
	
	FrameNum[0] = FrameNum[1];

	if ( !GoldenEvent )
	  {
	    cout << iEvtN << "\t\t" << store[0][i] << "\t" << short(previous[0]) << "  "
		 << (unsigned short)previous[1] << " " << (unsigned short)previous[2]
		 << "\t" << previous[3] << endl;
	  }
	else
	  {
	    cout << iEvtN << "\t\t" << store[0][i] << "\t" << extrema[0] << "  " << extrema[1] << " " << extrema[2] << "\t" << prob[i] << endl;
	    previous[3] = prob[i];
	    previous[0] = double(extrema[0]);
	    previous[1] = double(extrema[1]);
	    previous[2] = double(extrema[2]);
	  }
	/*printf("%.0f\t\t%.0f  %.0f %.0f\t%.0f  %.0f %.0f\t%.0f  %.0f %.0f\t\t%.0f  %.0f %.0f\t%.0f  %.0f %.0f\t%.0f  %.0f %.0f\t\t%.0f %.0f %.0f\t%.0f %.0f %.0f\t\t%.0f\t%.12f\n",store[0][i],
	  store[1][i],store[2][i],store[3][i],store[4][i],store[5][i],store[6][i],store[7][i],store[8][i],store[9][i],
	  store[10][i],store[11][i],store[12][i],store[13][i],store[14][i],store[15][i],store[16][i],store[17][i],store[18][i],
	  store[19][i],store[20][i],store[21][i],
	  store[22][i],store[23][i],store[24][i],store[25][i],prob[i]);*/
      } /* if GoldenEvent || ... */
  } /* RewFram to < NumFrames-ForFram-2 loop */ 
  return 0;
} /*main*/
