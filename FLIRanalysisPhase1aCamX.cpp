
#include <string>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <math.h>
#include <sstream>
#include <vector>

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
int width, height;

unsigned char* readBMP(char* filename)
{
  int i;
  FILE* f = fopen(filename, "rb");
  unsigned char info[54];

  // read the 54-byte header
  fread(info, sizeof(unsigned char), 54, f);

  // extract image height and width from header
  width = *(int*)&info[18];
  height = *(int*)&info[22];
  //cerr << width << " " << height << endl;

  // allocate three bytes per pixel
  int size = 3 * width * height;
  unsigned char* data = new unsigned char[size];

  // read the rest of the data at once
  fread(data, sizeof(unsigned char), size, f);
  fclose(f);

  for(i = 0; i < size; i += 3)
    {
      // flip the order of every 3 bytes
      unsigned char tmp = data[i];
      data[i] = data[i+2]; data[i+2] = tmp;
    }

  return data;

}

int main ( int argc, char** argv ) {
  
  unsigned long start = (unsigned long)atof(argv[1]);
  unsigned long end = start + (unsigned long)atof(argv[2]);
  bool CC = atoi(argv[3]);
  
  char line[80]; double temp; vector<double> CamSett;
  FILE *file = fopen("CamSett.txt","r");
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
  
  for ( unsigned long k = start; k < end; ++k ) {
    
    char* fileNameNew;
    stringstream temp; //stringstream().swap(temp);
    if ( k < 9 ) temp << "bitmaps/thumb00000" << (k+1) << ".bmp";
    else if ( k < 99 ) temp << "bitmaps/thumb0000" << (k+1) << ".bmp";
    else if ( k < 999 ) temp << "bitmaps/thumb000" << (k+1) << ".bmp";
    else if ( k < 9999 ) temp << "bitmaps/thumb00" << (k+1) << ".bmp";
    else if ( k < 99999 ) temp << "bitmaps/thumb0" << (k+1) << ".bmp";
    else temp << "bitmaps/thumb" << (k+1) << ".bmp";
     string name;
    temp >> name;
    fileNameNew = &name[0];
    unsigned char* dataNew = readBMP ( fileNameNew );
    unsigned char* dataOld;
    
    if ( k > start ) {
       char* fileNameOld;
      stringstream temp2; //stringstream().swap(temp);
      if ( k < 10 ) temp2 << "bitmaps/thumb00000" << k << ".bmp";
      else if ( k < 100 ) temp2 << "bitmaps/thumb0000" << k << ".bmp";
      else if ( k < 1000 ) temp2 << "bitmaps/thumb000" << k << ".bmp";
      else if ( k < 10000 ) temp2 << "bitmaps/thumb00" << k << ".bmp";
      else if ( k < 100000 ) temp2 << "bitmaps/thumb0" << k << ".bmp";
      else temp2 << "bitmaps/thumb" << k << ".bmp";
       name.clear();
      temp2 >> name;
      fileNameOld = &name[0];
      dataOld = readBMP ( fileNameOld );
    }
    
    int maximum[3] = {-1,-1,-1}; int minimum[3] = {256,256,256}; int maxLoc[3][2] = {0}; int minLoc[3][2] = {0};
    int NumPixAbvThr[3][2] = {0};
    int NumPixAbvSubThrSum = 0, CloudCover = 100;
    
    for ( int i = (height-1-CROP_XI); i >= (height-CROP_XF); --i ) {
      for ( int j = CROP_YI; j < CROP_YF; ++j ) {
	
	int rgbColorNew[3], rgbColorOld[3];
	
	if ( k == start ) {
	  rgbColorOld[0] = 0;
          rgbColorOld[1] = 0;
          rgbColorOld[2] = 0;
        }
	else {
	  rgbColorOld[0] = (int)dataOld[3 * (i * width + j) + 0];
	  rgbColorOld[1] = (int)dataOld[3 * (i * width + j) + 1];
	  rgbColorOld[2] = (int)dataOld[3 * (i * width + j) + 2];
	}
	
	rgbColorNew[0] = (int)dataNew[3 * (i * width + j) + 0];
        rgbColorNew[1] = (int)dataNew[3 * (i * width + j) + 1];
        rgbColorNew[2] = (int)dataNew[3 * (i * width + j) + 2];
	
	diffs[0] = rgbColorNew[0]-rgbColorOld[0];
	diffs[1] = rgbColorNew[1]-rgbColorOld[1];
	diffs[2] = rgbColorNew[2]-rgbColorOld[2];
	int jj = j;
	int ii = height - 1 - i;
	
	bool ExclusionZone = false;
	
#ifdef CamA1
	if ( abs(jj-1205) < 40 || (abs(jj-138) < 100 && abs(ii-30) < 25) || (abs(jj-1128) < 130 && abs(ii-70) < 70) || ((jj-637)*(jj-637)+(ii-363)*(ii-363)) < 10000 ) ExclusionZone = true;
#endif
#ifdef CamA2
	if ( abs(jj-1120) < 100 || (abs(jj-323) < 280 && abs(ii-39) < 40) || (abs(jj-1055) < 170 && abs(ii-49) < 50) || (abs(jj-650) < 115 && abs(ii-353) < 75) ||
	     ((jj-640)*(jj-640)+(ii-130)*(ii-130)) < 10000 ) ExclusionZone = true;
#endif
#ifdef CamA3
	if ( abs(jj-1163) < 100 || (abs(jj-133) < 100 && abs(ii-30) < 25) || (abs(jj-1128) < 130 && abs(ii-36) < 35) || ((jj-635)*(jj-635)+(ii-357)*(ii-357)) < 10000 ) ExclusionZone = true;
#endif
#ifdef CamA4
	if ( abs(jj-1123) < 100 || (abs(jj-323) < 278 && abs(ii-39) < 40) || (abs(jj-1055) < 170 && abs(ii-48) < 50) || ((jj-630)*(jj-630)+(ii-351)*(ii-351)) < 10000 ) ExclusionZone = true;
#endif
#ifdef CamB1
	if ( abs(jj-1123) < 98 || (abs(jj-650) < 340 && abs(ii-Ret[0]) < Ret[1]) || (abs(jj-323) < 278 && abs(ii-38) < 37) || ( jj > 1025 && ii > 565 ) || (abs(jj-1058) < 163 && abs(ii-50) < 50) ||
             ((jj-649)*(jj-649)+(ii-362)*(ii-362)) < 6400 || ((jj-885)*(jj-885)+(ii-205)*(ii-205)) < 900 || ((jj-408)*(jj-408)+(ii-205)*(ii-205)) < 529 ) ExclusionZone = true;
#endif
#ifdef CamB2
	if ( abs(jj-1205) < 45 || (abs(jj-135) < 100 && abs(ii-28) < 27) || (abs(jj-1163) < 100 && abs(ii-37) < 34) ) ExclusionZone = true;
#endif
#ifdef CamB3
	if ( abs(jj-1135) < 85 || (abs(jj-323) < 278 && abs(ii-40) < 36) || (abs(jj-1058) < 170 && abs(ii-70) < 25) || (abs(jj-640) < 115 && abs(ii-358) < 75) ) ExclusionZone = true;
#endif
#ifdef CamB4
	if ( abs(jj-1203) < 45 || (abs(jj-138) < 100 && abs(ii-30) < 25) || (abs(jj-1128) < 130 && abs(ii-65) < 65) || ((jj-637)*(jj-637)+(ii-364)*(ii-364)) < 11000 ) ExclusionZone = true;
#endif


#ifdef Custom
	if ( false /*
                      Here you can put a predicate to be true if (ii,jj) is a Custom exclusion zone,
                      We have none, so false serves as a placeholder.
                      ExclusionZone was already initialized to false.
		   */
	     ) ExclusionZone = true;
#endif
	
	if ( !ExclusionZone || k == start ) {
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
    
    if ( k > start ) {
      fprintf(stdout,"%lu\t\t%i  %i %i\t%i  %i %i\t%i  %i %i\t\t%i  %i %i\t%i  %i %i\t%i  %i %i\t\t%i %i %i\t%i %i %i\t\t%d\n",
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
    }
    else {
      
      /*fprintf(stderr,
	      "The absolute (no subt) details from the first frame:\n%lu\t\t%i  %i %i\t%i  %i %i\t%i  %i %i\t\t%i  %i %i\t%i  %i %i\t%i  %i %i\t\t%i %i %i\t%i %i %i\t\t%d\n",k,
             maximum[0],maxLoc[0][1],maxLoc[0][0],maximum[1],maxLoc[1][1],maxLoc[1][0],maximum[2],maxLoc[2][1],maxLoc[2][0],
             minimum[0],minLoc[0][1],minLoc[0][0],minimum[1],minLoc[1][1],minLoc[1][0],minimum[2],minLoc[2][1],minLoc[2][0],
             NumPixAbvThr[0][0],NumPixAbvThr[1][0],NumPixAbvThr[2][0],
             NumPixAbvThr[0][1],NumPixAbvThr[1][1],NumPixAbvThr[2][1],NumPixAbvSubThrSum);*/

      /* #ifdef CamB1
	 cerr << NumPixAbvThr[0][0]+NumPixAbvThr[1][0]+NumPixAbvThr[2][0] << endl;
	 if ( (NumPixAbvThr[0][0]+NumPixAbvThr[1][0]+NumPixAbvThr[2][0]) > CloudCover || CC )
	 { Ret[0] = 360; Ret[1] = 230; }
	 #else
	 cerr << CloudCover - CloudCover << endl; 
         #endif 
      */

    }
    
  } /* end of frame loop */
  
  return 0;
  
} /* end of main */
