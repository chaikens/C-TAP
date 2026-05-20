#include <iostream>
using namespace std;

int a, b;

int fun(void)
{
  return 9;
}

int g = fun();

int main(int argc, char *argv[])
{
  cout << g << endl;
  return 0;
}
