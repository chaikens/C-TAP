#include "bmp.h"
#include <iostream>
#include <random>
using namespace std;

int main(int argc, char **argv)
{
  std::random_device rd;  // a seed source for the random number engine
  std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()
  std::uniform_int_distribution<> distrib(0, 255);
  FULLBITMAP *p = makeClassicBitmap(1024, 1024);
  
  for(int i = 0; i<1024*1024*3; i++)
    {
      p->bytes[i] = distrib(gen);
    }

  FILE* pf = fopen("RandomImage.bmp", "w");
  fwrite(p, p->headers.fileHeader.FileSize, 1, pf);
  fclose(pf);
  return 0;
}

	
