typedef uint16_t pixCoord;
float UtoPFloat(float c);

/*************************   EXCLUSION ZONING ************************************
 ******  implemented in EZ bottom trapezoid class class EZvtraps in EZbtraps.h ***
 
| xL  yL 1 |   |    xL     yL    1 | = (xL)*(yR-yL)-(yL)*(xR-xL) + (xR-xL)*y - (yR-yL)*x
| xR  yR 1 | = | xR-xL  yR-yL    0 | = (xL)*(  yD )-(yL)*(  xD ) + (  xD )*y - (  yD )*x
| x   y  1 |   |    x      y     1 |

 xLi <= x <= xRi

NEXT[ (xRi, yRi),          (xD(i+1),yD(i+1))   ] =
    ( xR(i+1)=xRi+xD(i+1), yR(i+1)=yRi+yD(i+1) )

Initialize an instance with numbers from an .svg <path> elt. d attrib,
where d="m sx,sy d1x,d1y d2x,d2y ..." like

EZbtraps BenCambtraps =
  { . xL0 = 0, . yL0 = 360 ,  //Coordinates of first point
    . diffs = {
      {121.33169,187.96547 }, //offsets from previous point to this point
      {300,65.37608 },
      {61.33169,245.3021 },
      {77.31196,2.66338 },
      {44.11837,-161.47966 },
      {133.26757,-182.49075 },
      {25.32676,146.88039 },
      {6.78175,183.57584 },
      {25.40074,-11.7386 },
      {65.91861,-130.55487 },
      {58.18745,0.66585 },
      {38.41553,133.50185 },
      {961.27618,2.9963},
      {0,0}}                 //unlike .svg, we terminate with xdiff=0.
  };

and use like

static bool ezBenCam( pixCoord ii, pixCoord jj)
{ return
    ((ii < UtoP(73) ) && (jj > UtoP(1568) )
    ||
    BenCambtraps.ez(jj, ii) );
}
(F1a code uses ii for y pix coods, jj for x pix coord)

*/

class EZbtraps {
 public: 
  const float xL0, yL0;
  const float diffs[][2];
  bool ez(pixCoord x, pixCoord y)
  {
    float xL = xL0;
    float yL = yL0;
    int i = 0;
    while (diffs[i][0] != 0.0)
      {
	float xD = diffs[i][0]; //these must positive until terminated by 0.0
	float yD = diffs[i][1]; //could be +, - or 0
	i++;
	float xR = xL + xD;
	float yR = yL + yD;
	//C++ trap: Unlike Python, ( A <= x <= B ) doesn't work!
	if ( (UtoPFloat(xL) <= x) && (x <= UtoPFloat(xR)) )
	  {
	    if( (- UtoPFloat(yD)*((float)x)
		 + UtoPFloat(xD)*((float)y)
		 + UtoPFloat(yD)*UtoPFloat(xL)
		 - UtoPFloat(yL)*UtoPFloat(xD)) >= 0)
	      {
		return true;
	      }
	    else
	      {
		return false;
	      }
	  }
	xL = xR;
	yL = yR;
      }
    return false;
  }
};
