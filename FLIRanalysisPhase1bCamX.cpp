
#include <fstream>
#include <cstdio>
#include <iostream>
#include <algorithm>
#include <stdlib.h>
#include <cmath>
#include <math.h>
#include <vector>
#include <string>
#include <getopt.h>
using namespace std;

// Command line options.  Set when, early, main calls
static int get_our_options( int *argc, char **argv[]);

static const char *CamSett_file = "CamSett.txt";
//static const char *CamSett_file_tmp = "CamSett.txt.tmp";
static const char *new_CamSett_file = 0;
//static const char *SYS_CamSettMOVE ="mv CamSett.txt.tmp CamSett.txt";
// About CamSett
// Code below to modify CamSett.txt is buggy
// and commented out until Matt advises on a
// better Camera property management system.

static const char *pix_scale_string = 0;

//scaling?  typedef unsigned short pixCoord; not done here
vector<bool> SignalTruth;
double prob[555000];

int main ( int argc, char** argv ) {
  
  get_our_options( & argc, & argv);
  // --CamSett-file is really an option.
  // if not supplied, the original defaults that work when
  // these files are in cwd are used.

  vector<short> extrema;
  unsigned long i = 0;
  signed short j = 0;
  float AbsAvg, AbsStdDev, FracYes;
  
  char line[80];
  double temp;
  //Read CamSett.txt -- keep.
  vector<double> CamSett;
  int RewFram, ForFram, FramBefNew, SubThr;
  FILE *file = fopen(CamSett_file,"r");
  //FILE *note = fopen(CamSett_file_tmp,"w");
  cerr << "endl" << "Reading " << CamSett_file << endl;
  for ( j = 0; j < 20; ++j ) {
    fscanf ( file, "%s %lf", line, &temp );
    CamSett.push_back(temp);
    cerr << "Line" << j << " Value=" << temp << "(ignored Key =" << line << ")" << endl;
  }
  /***********************************************
  From original CamSett.txt
Line0 Value=0(ignored Key =smallestThr=)
Line1 Value=254(ignored Key =biggestThr=)
Line2 Value=0(ignored Key =smallestPix=)
Line3 Value=67(ignored Key =biggestPix=)
Line4 Value=0.633(ignored Key =SkewGaussAmpl=)
Line5 Value=1.97(ignored Key =SkewGaussXi=)
Line6 Value=1.89(ignored Key =SkewGaussOmega=)
Line7 Value=2.5(ignored Key =SkewGaussAlpha=)
Line8 Value=0(ignored Key =NumPixAbvThrSumMin=)
Line9 Value=3(ignored Key =NumPixAbvThrSumMax=)
Line10 Value=20(ignored Key =SubThr=)
Line11 Value=0(ignored Key =RewFram=)
Line12 Value=-2(ignored Key =ForFram=)
Line13 Value=1(ignored Key =FramBefNew=)
Line14 Value=1(ignored Key =FracYes=)
Line15 Value=100(ignored Key =CROP_XI=)
Line16 Value=900(ignored Key =CROP_XF=)
Line17 Value=0(ignored Key =CROP_YI=)
Line18 Value=1919(ignored Key =CROP_YF=)
Line19 Value=14(ignored Key =mainThreshold=)
  **************************************************/
  char camName[80];  //used to be [3], caused buffer overflow!
  fscanf ( file, "%s %s", line, camName );
  string camera(camName);
  cerr << "Last read:" << line << " camName=" << camName << " C++string=" << camera << endl;
  //rewind(file);
  //for ( j = 0; j < 20; ++j ) {
  //fgets(line, sizeof(line), file);
  //fputs(line, note);
  // }
  fclose(file);
  //fclose(note);
  //fprintf(stderr, "Before calling system, SYS_CamSettMOVE=%s\n", SYS_CamSettMOVE);
  //system(SYS_CamSettMOVE);
  string CapParams = "(initial value)";
  if ( camera == "Custom" ) {
    SubThr = (int)CamSett[10];
    RewFram= (int)CamSett[11];
    ForFram =(int)CamSett[12];
    FramBefNew = (int)CamSett[13];
    FracYes = (float)CamSett[14];
    CapParams = "From Custom";
  }
  else { CapParams = "From defaults";
    RewFram = 100; ForFram = 320; FramBefNew = 320; SubThr = 20; FracYes = 0.07; }

  string boundsParams = "(initial value)";
  int smallestThr, biggestThr, smallestPix, biggestPix;
  if ( camera == "B1" || camera == "A3" ) { // "bad" camera
    smallestThr = 34;
    biggestThr = 54;
    smallestPix = 3;
    biggestPix = 30;
    boundsParams = "From camera B1 or A3, bad camera";
  }
  else if ( camera == "B3" || camera == "B4" ) { // "medium"-quality cam
    smallestThr = 0;
    biggestThr = 254;
    smallestPix = 0;
    biggestPix = 69; //set to min allowed for catching object starting at frame 41935 in camB3 (14_00-59-40). Line 197 makes this hist
    boundsParams = "From camera B3 or B4, medium quality";
  }
  else if ( camera == "A1" || camera == "A2" || camera == "A4" || camera == "B2" ) { // "good" camera
    smallestThr = 0;
    biggestThr = 254;
    smallestPix	= 0;
    biggestPix = 300; //200 is min allowed to catch object at 09:20:48-50am in CamA4 (July 15)
    boundsParams = "From camera A1 or A2 or A4 or B2, good camera";
  }
  else {
    smallestThr = (int)CamSett[0];
    biggestThr = (int)CamSett[1];
    smallestPix = (int)CamSett[2];
    biggestPix = (int)CamSett[3];
    boundsParams = "From CamSett.txt";
  }

  cerr << endl << "Phase1b Capitalized Params: " << CapParams << endl;
  cerr << "SubThr=" << SubThr << endl;
  cerr << "RewFram=" << RewFram << endl;
  cerr << "ForFram=" << ForFram << endl;
  cerr << "FramBefNew=" << FramBefNew << endl;
  cerr << "FracYes=" << FracYes << endl;
  cerr << endl;
  cerr << "Phase1b bounds Params: " << boundsParams << endl;
  cerr << "smallestThr" << smallestThr << endl;
  cerr << "biggestThr=" << biggestThr << endl;
  //scaling?
  cerr << "smallestPix=" << smallestPix << endl;
  cerr << "biggestPix=" << biggestPix << endl;
  cerr << endl;
  
  //
  // Process non-option command line args.
  //
  //One could read (filename argv[1]) until EOF
  //to count or check NumFrames==number of difference lines.
  ifstream ifp(argv[1]); long NumFrames = atol(argv[2]);

  double level = atof(argv[3]);

  if ( level >= 0.979 && camera != "B1" ) {
    if ( camera == "B2" ) level = 0.999999999999; // the minimum allowed that doesn't make B2 collect too many events (16_21-57-25)
    else if ( camera == "B3" ) level = 0.9999999999;
    else if ( camera == "A3" ) level = 0.9999999; // 15_19-57-27 8:51:30-53pm (boat?)
    else if ( camera == "Custom" ) { ; }
    else level = 0.99999999999; // 11 9's
  }
  //
  //It might be better to compute with log(1 - level) since it's so tiny sometimes.
  //
  //At present, level:=0.5 for first try and 0.97 for redo.
  //Also, this kind of adjustment for special movie/camera issues may be better
  //handled at the script level.
  //
  // level is used only once, in the diff frame traversing loop after the reading and mean loop.
  //
  
  double data[26];  //for reading Phase1a data.
  double GlobPixMean[7] = {0.,0.,0.,0.,0.,0.,0.};
  vector<double> store[26];
  //for storing that data, store[kind of data][which diff frame]
  //(?Not very important) store[] starts at 0 but diff line numbers at the
  //start of each line begin at 1, for frame1-frame0.
  
  for ( i = 0; i < NumFrames; ++i ) {
    ifp >> data[0] >> data[1] >> data[2] >> data[3] >> data[4]
	>> data[5] >> data[6] >> data[7] >> data[8] >> data[9]
	>> data[10] >> data[11] >> data[12] >> data[13] >> data[14]
	>> data[15] >> data[16] >> data[17] >> data[18] >> data[19]
	>> data[20] >> data[21] >> data[22] >> data[23] >> data[24] >> data[25];
    
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

    //C-TAP=Custom Target Analysis Protocol
    // (Szydagis) arXiv:2312.00558v4
    // chaikens annotation/refactorization.
    // FLIR stands for Forward-Looking InfraRed, cameras for which
    // this project was started, but then extended to regular cameras.
    //
    //Page11 par1,3: min and max diffs in pixels between successive frames.
    //
    //       par4 L1 Calc avg in pixel diffs.
    //
    //       par3 L3-5:
    //"It is adjusted depending on the nature of the noise, such as a
    //severe non-Gaussian upward tail, typical of the older (noisier) camera units,
    //necessitating setting a threshold toward the upper end of this range to avoid
    //a high false-positive rate."
    //
    //data[] includes differences; from current frame data[0] - previous frame
    // starting with frame1 (2nd movie frame) - (beginning frame)
    // i  data[i] meaning                       we compute                  notation in paper
    //
    // 0  difference frame number (starts with 1) 
    // 1  max pos diff Red   (max +R)        its mean=:GlobPixMean[0]
    // 2      where x	    
    // 3      where y	    
    // 4  max pos diff Green (max +G)        its mean=:GlobPixMean[1]
    // 5      where x        
    // 6      where y
    // 7  max pos diff Blue  (max +B)        its mean=:GlobPixMean[2]
    // 8     where x	    
    // 9     where y

    //10  min neg diff Red, neg#. (min -R)  (- its mean)=:GlobPixMean[3](>=0)
    //11      where x	    
    //12      where y	    
    //13  max pos diff Green, neg#. (min -G)(- its mean)=:GlobPixMean[4](>=0)
    //14      where x        
    //15      where y
    //16  max pos diff Blue, neg#  (min +B) (- its mean)=:GlobPixMean[5](>=0)
    //17     where x	    
    //18     where y
    
    //19  #pixels with +R >= (max +R) - thresh
    //20  #pixels with +G >= (max +G) - thresh
    //21  #pixels with +B >= (max +B) - thresh
    //22  #pixels with -R <= (min -R) + thresh
    //23  #pixels with -G <= (min -G) + thresh
    //24  #pixels with -B <= (min -B) + thresh

    //25  var name used below:NumPixAbvSubThrSum    its mean=:GlobPixMean[6]

  }
  ifp.close();
  
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
  for ( i = 0; i < NumFrames; ++i )
  GlobPixSigma += pow(GlobPixMean[6]-store[25][i],2.);
  GlobPixSigma /= (double(NumFrames)-1.);
  GlobPixSigma = sqrt(GlobPixSigma);
  fprintf(stderr,"The number of pixels above %d units is %.2f +/- %.2f\n",SubThr,GlobPixMean[6],GlobPixSigma);
  // used only once, -------V---, from CamSett and our changes above
  int MinPix = std::max(smallestPix,int(ceil(GlobPixMean[6]+1.)));
  // used only once--V--,         from CamSett and our changes above
  int MaxPix = biggestPix;
  while ( MaxPix <= MinPix ) {
    if ( camera == "B1" )
      return 1;
    else
      MaxPix += 5;
  }
  fprintf(stderr,"Minimum number of pixels allowed to be above the sub-threshold of %d is %d\n",SubThr,MinPix);
  fprintf(stderr,"Maximum number of pixels allowed to be above the sub-threshold of %d is %d\n",SubThr,MaxPix);

  //big diff frame loop
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

    //new use of extrema, separate for each diff frame i
    extrema.clear();
    extrema.push_back(maxR); //inserts at posn [len]
    extrema.push_back(maxG);
    extrema.push_back(maxB);
    extrema.push_back(maxr);
    extrema.push_back(maxg);
    extrema.push_back(maxb);
    std::sort( extrema.begin(), extrema.end() ); //sorted goes up or ==, non-descending sort.
    unsigned short AbsMax =abs(extrema.back());
    //Apparently, C++ doesn't have vararg max( ., ., ., ... )
    //this extrema data not used anymore

    
    AbsAvg = ( maxR + maxG + maxB + maxr + maxg + maxb ) / 6.;
    AbsStdDev = ( maxR - AbsAvg ) * ( maxR - AbsAvg ) + ( maxG - AbsAvg ) * ( maxG - AbsAvg ) + ( maxB - AbsAvg ) * ( maxB - AbsAvg ) +
      ( maxr - AbsAvg ) * ( maxr - AbsAvg ) + ( maxg - AbsAvg ) * ( maxg - AbsAvg ) + ( maxb - AbsAvg ) * ( maxb - AbsAvg );
    AbsStdDev = sqrt ( AbsStdDev / 5. );
    
    vector<double> SkewGauss(4); // "classic" values for the next line: ampl .633, mu 1.97, sig 1.89, skew 2.5
    if ( camera == "Custom" ) {
      SkewGauss[0] = CamSett[4]; //SkewGaussAmpl 
      SkewGauss[1] = CamSett[5]; //SkewGaussXi	 
      SkewGauss[2] = CamSett[6]; //SkewGaussOmega
      SkewGauss[3] = CamSett[7]; //SkewGaussAlpha
    }
    else {
      SkewGauss[0] = 0.673;  //SkewGaussAmpl 
      SkewGauss[1] = 2.;     //SkewGaussXi	 
      SkewGauss[2] = 2.;     //SkewGaussOmega
      SkewGauss[3] = 2.0;    //SkewGaussAlpha
    }
    
    prob[i] = 1. -
      SkewGauss[0]
      *( exp( -0.5*(AbsStdDev-SkewGauss[1])*(AbsStdDev-SkewGauss[1])
	           /(SkewGauss[2]*SkewGauss[2]) ) )
      *(        1. + erf(SkewGauss[3]*(AbsStdDev-SkewGauss[1])/(SkewGauss[2]*sqrt(2.)))          );

    //
    // decision below depends on camera
    //
    // level is used only one time, here:
    //  --------------V--
    if ( (prob[i] > level && (AbsMax > MinThr && AbsMax < MaxThr) && NumPixAbvSubThrSum > MinPix && NumPixAbvSubThrSum < MaxPix) ||
	 (NumPixAbvThrSum > 36 && NumPixAbvThrSum < 44 && camera == "B1") || (NumPixAbvThrSum > 100 && (camera == "B3" || camera == "B4")) ||
	 (camera == "Custom" && NumPixAbvThrSum > CamSett[8] && NumPixAbvThrSum < CamSett[9]) )
      SignalTruth.push_back(true);
    else
      SignalTruth.push_back(false);
    //end of big diff frame loop on i
  }
  
  double previous[4] = {1.,256.,0.,0.};
  unsigned short int iEvtN = 0; // the global event number
  unsigned short FrameNum[2] = { 0, 0 };
  
  if ( camera != "Custom" ) {
    SignalTruth.erase(SignalTruth.begin());
  }
  
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
    
    if ( GoldenEvent || float(NumBools) / float(ForFram+RewFram) > FracYes ) {
      unsigned short maxR = (unsigned short)store[1][i];
      unsigned short maxG = (unsigned short)store[4][i];
      unsigned short maxB = (unsigned short)store[7][i];
      unsigned short maxr = (unsigned short)(-store[10][i]);
      unsigned short maxg = (unsigned short)(-store[13][i]);
      unsigned short maxb = (unsigned short)(-store[16][i]);
      
      //new use of extrema
      extrema.clear();
      extrema.push_back(maxR); extrema.push_back(maxG); extrema.push_back(maxB); extrema.push_back(maxr); extrema.push_back(maxg); extrema.push_back(maxb);
      std::sort(extrema.begin(),extrema.end());
      unsigned short AbsMax = abs(extrema.back());
      extrema.clear();
      //Just compute max's?
      //done with extrema

      //new use of extrema, not for finding max, but for reporting events
      //(vector).push_back(val) inserts at posn [len]
      if ( AbsMax == maxR ) { extrema.push_back( maxR); extrema.push_back(store[2][i]); extrema.push_back(store[3][i]); }
      else if ( AbsMax == maxG ) { extrema.push_back( maxG); extrema.push_back(store[5][i]); extrema.push_back(store[6][i]); }
      else if ( AbsMax == maxB ) { extrema.push_back( maxB); extrema.push_back(store[8][i]); extrema.push_back(store[9][i]); }
      else if ( AbsMax == maxr ) { extrema.push_back(-maxr); extrema.push_back(store[11][i]); extrema.push_back(store[12][i]); }
      else if ( AbsMax == maxg ) { extrema.push_back(-maxg); extrema.push_back(store[14][i]); extrema.push_back(store[15][i]); }
      else { extrema.push_back(-maxb); extrema.push_back(store[17][i]); extrema.push_back(store[18][i]); }

      FrameNum[1] = store[0][i];
      if ( (FrameNum[1]-FrameNum[0]) > FramBefNew && FrameNum[0] != 0 ) ++iEvtN;
      FrameNum[0] = FrameNum[1];
      
      if ( !GoldenEvent ) {
	cout << iEvtN << "\t\t" << store[0][i] << "\t" << short(previous[0]) << "  " << (unsigned short)previous[1] << " " << (unsigned short)previous[2] << "\t" << previous[3] << endl;
      }
      else {
	cout << iEvtN << "\t\t" << store[0][i] << "\t" << extrema[0] << "  " << extrema[1] << " " << extrema[2] << "\t" << prob[i] << endl;
	previous[3] = prob[i];
	previous[0] = double(extrema[0]);
	previous[1] = double(extrema[1]);
	previous[2] = double(extrema[2]);
      }
      //end of use of this extrema
      
      /*printf("%.0f\t\t%.0f  %.0f %.0f\t%.0f  %.0f %.0f\t%.0f  %.0f %.0f\t\t%.0f  %.0f %.0f\t%.0f  %.0f %.0f\t%.0f  %.0f %.0f\t\t%.0f %.0f %.0f\t%.0f %.0f %.0f\t\t%.0f\t%.12f\n",store[0][i],
	     store[1][i],store[2][i],store[3][i],store[4][i],store[5][i],store[6][i],store[7][i],store[8][i],store[9][i],
	     store[10][i],store[11][i],store[12][i],store[13][i],store[14][i],store[15][i],store[16][i],store[17][i],store[18][i],
	     store[19][i],store[20][i],store[21][i],
	     store[22][i],store[23][i],store[24][i],store[25][i],prob[i]);*/
      
      //end of  if ( GoldenEvent || float(NumBools) / float(ForFram+RewFram) > FracYes )
    }
    //end of loop ( i = RewFram; i < (NumFrames-ForFram-2); ++i ) 
  }
  
  return 0; //exit main, we're done.
}


static int get_our_options( int *argc, char **argv[])
{
  extern char *optarg; //globals from getopt and getopt_long
  extern int optind, opterr, optopt;

  int c;
  int digit_optind = 0;

  //Logic here adapted from man 3 getopt
  while (1) {
    int this_option_optind = optind ? optind : 1;
    int option_index = 0;
    static struct option long_options[] = {
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
      case 0: new_CamSett_file = optarg;
	break;
      case 1:
	pix_scale_string = optarg;
	break;
      }
    }
  }

  char *cmd = (*argv)[0];
  *argc = *argc - (optind - 1);
  *argv = *argv + (optind - 1);
  (*argv)[0] = cmd;

  if(new_CamSett_file) {
    //fprintf(stderr, "new_CamSett_file %s\n", CamSett_file);
    
    CamSett_file = new_CamSett_file;
    //string CamSett_file_string = CamSett_file;
    //string tstr = string(CamSett_file) + ".tmp ";
    //string mv = "mv ";
    //string mvCmd = mv + tstr + CamSett_file_string;
    //cerr << "move command=" << mvCmd << endl;

    //SYS_CamSettMOVE = mvCmd.c_str();
    //fprintf(stderr, "from mvCmd.c_str():%s\n", SYS_CamSettMOVE);

  }


  //string bitmaps_dir_string = bitmaps_dir;
  //bmp_file_printf_format_string = bitmaps_dir_string + "/" + thumb_format;
  //bmp_file_printf_format_cstring = bmp_file_printf_format_string.c_str();
  //bmpfilename_buffer =
  //  new char[bmp_file_printf_format_string.length()
  //	     + 2*5/*for safe paranoia*/];

  return 0;
}
