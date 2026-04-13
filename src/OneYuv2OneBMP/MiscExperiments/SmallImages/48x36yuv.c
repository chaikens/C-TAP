#include <stdio.h>

unsigned char yuv[] =
  { 10, 11, 12, 13,
    14, 15, 16, 17,
    20, 22,
    30, 32};

int main()
{
  for ( unsigned int i=0; i<48*36+24*36 ;  i++ ) putchar(i);
  return 0;
}
