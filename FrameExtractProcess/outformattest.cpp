#include <stdio.h>

const char *fm = "%*d %d %*d %*d %*d %*f";
int main( int argc, char *argv[])
{ int x;
  if(argv[1] != 0)
    { fm = argv[1]; }
  printf("%s\n", fm);
  while(1 == scanf(fm, &x)) {
    printf("%d ", x);
  }
}
