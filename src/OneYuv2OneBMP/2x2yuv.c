#include <stdio.h>

unsigned char yuv[] = { 100, 101, 110, 111, 125, 175 };
int main()
{
  for ( unsigned int i=0; i<sizeof(yuv) ;  i++ ) putchar(i);
  return 0;
}
