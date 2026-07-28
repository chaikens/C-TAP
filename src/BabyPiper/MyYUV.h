#include <iostream>
using namespace std;

class MyYUV {
  unsigned int width;
  unsigned int height;
  uint8_t *bytes;
  uint8_t Ycirc = 200;
  uint8_t Ucirc = 172;
  uint8_t Vcirc = 84;
 public:
 MyYUV( unsigned int width, unsigned int height, uint8_t *bytes) :
  width(width), height(height), bytes(bytes)
    { }
  void setY( unsigned int x, unsigned int y, uint8_t val )
  {
    if ( x >= width || y >= height ) {
      cerr << "Warning: MyYUV.setY called on out of range x,y = "
	   << x << ", " << y << " outside (" << width-1 << "," << height-1 << ")" << endl;
      return;
    }
    bytes[width*y + x] = val;
    return ;
  }
  void setY( unsigned int x, unsigned int y, uint8_t yval, uint8_t uval, uint8_t vval)
  {
    setY( x, y, yval );
    bytes[width*height+(width/2)*(y/2) + x/2]=uval;
    bytes[width*height + width*height/4 + (width/2)*(y/2) + x/2]=vval;
  }
  void circle( int xc, int yc, float rad, float thick ) {
    int left = imax(xc - rad, 0);
    int right = imin(xc + rad, width - 1);
    int top = imax(yc - rad, 0);
    int bot = imin(yc + rad, height - 1);
    //cerr << "LRTB " << left << " " << right << " " << top << " " << bot << endl;
    float rsqmin = (rad - thick/2)*(rad - thick/2);
    float rsqmax = (rad + thick/2)*(rad + thick/2);
    for( int x = left; x <= right; x++ ) {
      for( int y = top; y <= bot; y++ ) {
	float dsq = (x - xc)*(x - xc)+(y-yc)*(y-yc);
	//cerr << "x y dsq " << x << " " << y << " " << dsq << endl;
	//cerr << "rsq{min,max} " << rsqmin << " " << rsqmax << endl;

	if( (dsq >= rsqmin) && (dsq <= rsqmax) )
	  { //cerr << "Set " << x << ", " << y << endl;
	    setY( x, y, Ycirc, Ucirc, Vcirc );
	  }
	//else {
	//cerr << "DONT Set " << x << ", " << y << endl;
	//}
      }
    }
    return ;
  }
 private:
  static int imax( float a, float b )
  {
    if( a < b ) {return b;} else {return a;}
  }
  static int imin( float a, float b )
  {
    if( a < b ) {return a;} else {return b;}
  }
  
  
};

  
