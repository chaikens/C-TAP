#include <stdint.h>
#include <stddef.h>
#include <error.h>
#include <errno.h>
#include <iostream>
#include <stdio.h>

using namespace std;
class BitA
{ //SSSTTTuuuuuUUUUUpppPPPiiIIIDDDD
  //initialization order is order of DECLARATION!!!!
  //not the initializers.UUUGGHHH
  const size_t width, height, wh, size;
  uint8_t * const A;
 public:
  BitA(size_t width, size_t height)
    : width(width), height(height), wh(width*height),
    size((wh+7)/8), A(new uint8_t [size])
  { }
  void set(size_t r, size_t c)
  {
    size_t i = c + width*r;
    if ( i >= wh ) {
      error(1, 0, "Out of bounds BitA(%ld,%ld).set(%ld,%ld).", width, height,r,c);
    }
    //cerr << "Before A[" << i/8 << "]=" << A[i/8] << endl;
    A[i/8] |= (128 >> (i % 8));
    //cerr << " After  A[" <<  i/8 << "]=" <<  A[i/8] << endl;
  }
  void writefile( char *fn )
  {
    FILE *FP = fopen(fn, "w");
    if( FP == 0 ) {
      error(1, errno, "Failure to open bitarray file %s for writing.", fn);
    }
    if( 1 != fwrite(A, size, 1, FP) ) {
      error(1, errno, "Failure to write bitarray file %s.", fn);
    }
    if(fclose(FP)) {
      error(1, errno, "Failed to close bitarray file %s.", fn);
    }
  }
  ~BitA() {
    delete[] A;
  }
};
