
#include <stdio.h>
#include <stdint.h>
extern const uint8_t UnivI420pUnc4096x4096BMP;

int main(int argc, char *argv[])
{
  printf("hi\n");
  FILE *fp = fopen("try.bmp", "w");
  const uint8_t *pv = &UnivI420pUnc4096x4096BMP;
  fwrite( pv , 50331702, 1, fp);
  return 0;
}


  
