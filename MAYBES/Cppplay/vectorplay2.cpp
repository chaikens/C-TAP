#include <vector>
#include <iostream>
#include <algorithm>
#include <cmath>

using namespace std;

int main(int argc, char *argv[])
{
  vector<short> extrema = { 2, 8, 1, 9, 19, 6 };
  /*extrema.clear();
    extrema.push_back(2); //inserts at posn [0]
    extrema.push_back(8);
    extrema.push_back(1);
    extrema.push_back(9);
    extrema.push_back(19);
    extrema.push_back(6);
  */
  cout << *max_element(extrema.begin(), extrema.end()) << endl;
  cout << extrema[0] << " " << extrema[1] << " " << extrema[2] << endl;
    cout << ".back()= " << extrema.back() << endl;
    cout << "now sort" << endl;
    sort( extrema.begin(), extrema.end() );
    short AbsMax =abs(extrema.back());
    //cout << extrema << end;
    cout << AbsMax << endl;
    cout << extrema[0] << endl;
    cout << extrema[5] << endl;
    return 0;
}
