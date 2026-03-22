#include "OneYuv2OneBmp.h"
#include <iostream>
using namespace std;
int main(int argc, char **argv){
  uint8_t foo[88];

  int a; int b; int c; int d; int e; int f;
  cin >> a >> b >> c >> d >> e >> f;
  convert4toBMP(foo+a,
		foo+b,
		foo+c,
		foo+d,
		foo+e,
		foo+f);
  cout << foo;
  return 0;
}
  
