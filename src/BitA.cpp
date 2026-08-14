#include "BitA.h"

int main(int argc, char *argv[])
{
  BitA *mba = new BitA( 3, 4 );
  for( int r = 0; r < 4; r++)
    for( int c = 0; c < 3; c++)
      mba->set(r,c);
  return 0;
}
