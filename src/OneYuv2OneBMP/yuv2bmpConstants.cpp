
/*
BGRfromYUV calculated by Sage/Numpy
From https://learn.microsoft.com/en-us/windows/win32/medfound/about-yuv-video
new used for HDTV, ITU-R BT.709
Ypfla = 0.2125*R + 0.7154*G + 0.0721*B
#Old std def TV, TU-R BT.601
YpflaOld = 0.299*R + 0.587*G + 0.114*B

BGRFromYUV =
                  Y              U               V
array([ b [ 1.00000000e+00,  1.00000000e+00, -2.99122488e-17],
        g [ 1.00000000e+00, -1.00782779e-01, -2.97036623e-01],
        r [ 1.00000000e+00,  0.00000000e+00,  1.00000000e+00]])

*/


  //order of colors in each BMP pixel:
  const int b = 0; const int g = 1; const int r = 2;

/* NEW CONSTANTS 
  // conversion matrix acts from the left:
const float by = 0.8; const float bu = 1.0; const float bv = 0.0; const float b1 = 0.0;
const float gy = 1.0; const float gu = -0.10078*(0.8); const float gv = -0.29704*(0.8); const float g1 = 0.0;
const float ry = 1.0*0.4; const float ru = 0.0; const float rv = 1.0*0.4; const float r1 = 0.0;
*/

/* OLD Constants */
/*  // conversion matrix acts from the left:
const float by = 1.0; const float bu = 1.0; const float bv = 0.0; const float b1 = 0.0;
const float gy = 1.0; const float gu = -0.19408; const float gv = -0.50937; const float g1 = 0.0;
const float ry = 1.0; const float ru = 0.0; const float rv = 1.0; const float r1 = 0.0;
*/

/* From https://fourcc.org/fccyvrgb.php
This is the source of Juliens too!
B = 1.164(Y - 16)                   + 2.018(U - 128)
G = 1.164(Y - 16) - 0.813(V - 128) - 0.391(U - 128)
R = 1.164(Y - 16) + 1.596(V - 128)

Also 
Y  =      (0.257 * R) + (0.504 * G) + (0.098 * B) + 16

Cr = V =  (0.439 * R) - (0.368 * G) - (0.071 * B) + 128

Cb = U = -(0.148 * R) - (0.291 * G) + (0.439 * B) + 128
*/

const float by = 1.164; const float bu = 2.018; const float bv = 0.0; const float b1 = 1.164*(-16) + 2.018*(-128);
const float gy = 1.164; const float gu = -0.391; const float gv = -0.813; const float g1 = 1.164*(-16) + 0.813*128 + 0.391*128;
const float ry = 1.164; const float ru = 0.0; const float rv = 1.596; const float r1 = 1.164*(-16) + 1.596*(-128);



/* Sample application:
     // contributions from U and V common to our 4 pixels:
  float buv = bu*Ubyte[0] + bv*Vbyte[0] + b1;
  float guv = gu*Ubyte[0] + gv*Vbyte[0] + g1;
  float ruv = ru*Ubyte[0] + rv*Vbyte[0] + r1;

#define f2rgb( x ) (uint8_t ( x ))
  
  firstBMPPair[b] = f2rgb(buv + by*firstYPair[0]);
  firstBMPPair[g] = f2rgb(guv + gy*firstYPair[0]);
  firstBMPPair[r] = f2rgb(ruv + ry*firstYPair[0]);
  firstBMPPair[b+3] = f2rgb(buv + by*firstYPair[1]);
  firstBMPPair[g+3] = f2rgb(guv + gy*firstYPair[1]);
  firstBMPPair[r+3] = f2rgb(ruv + ry*firstYPair[1]);

  secondBMPPair[b] = f2rgb(buv + by*secondYPair[0]);
  secondBMPPair[g] = f2rgb(guv + gy*secondYPair[0]);
  secondBMPPair[r] = f2rgb(ruv + ry*secondYPair[0]);
  secondBMPPair[b+3] = f2rgb(buv + by*secondYPair[1]);
  secondBMPPair[g+3] = f2rgb(guv + gy*secondYPair[1]);
  secondBMPPair[r+3] = f2rgb(ruv + ry*secondYPair[1]);

#undef f2rgb
*/

