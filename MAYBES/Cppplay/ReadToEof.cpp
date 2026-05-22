#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char *argv[])
{
  int X;

  ifstream i("nost");

  if(!i.is_open())
    {
      cerr << "open file failed" << endl;
    }

  while ( (i >> X) ) {
    cout << X<< endl;
  }
  cout << "loop end" << endl;

}
