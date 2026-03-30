#include "bmp.h"
#include <error.h>
#include <errno.h>
#include <cstring>
#include <cstdlib>
#include <iostream>
using namespace std;
CLASSICBMPHEADERS *makeClassicHeaders( int width, int height)
{
  CLASSICBMPHEADERS *p = new CLASSICBMPHEADERS;
  BITMAP_FILEHEADER *pfh = &p->fileHeader;
  BITMAPINFOHEADER  *pinfo = &p->infoHeader;

  /* typedef struct __attribute__((packed)) BITMAP_FILEHEADER {
  unsigned char Signature[2];
  uint32_t FileSize;
  uint16_t Reserved1;
  uint16_t Reserved2;
  uint32_t OffsetToData;
  } BITMAP_FILEHEADER;
  */

  pfh->Signature[0] = 'B'; pfh->Signature[1] = 'M';
  pfh->FileSize = sizeof(CLASSICBMPHEADERS) + 3*abs(width)*abs(height);
  // reserved and other field were 0'd by C++
  pfh->OffsetToData = sizeof(CLASSICBMPHEADERS);

/*
typedef struct __attribute__((packed)) BITMAPINFOHEADER {
  int32_t MySize;            // Wikipedia says its signed. 
  int32_t Width;
  int32_t Height;
  uint16_t NColorPlanes;     // Wikipedia says must be 1 
  uint16_t BitsPerPixel;     // aka Depth 
  uint32_t CompressionMeth;  // 0=BI_RGB none, most common 
  uint32_t SizeOfRawBmpData; // could be 0 for BI_RGB 
  int32_t ResolutionHoriz;   // pixels/meter 
  int32_t ResolutionVert;
  uint32_t ColorsInPalette;  // 0 to default to 2^n 
  uint32_t NImportantColors; // Usually ignored, 0 if all colors are used. 
  } BITMAPINFOHEADER;
*/

  pinfo->MySize = sizeof(BITMAPINFOHEADER);
  pinfo->Width  = width;     // Variable!
  pinfo->Height = height;     // Variable!
  pinfo->NColorPlanes = 1;
  pinfo->SizeOfRawBmpData = 3*abs(width)*abs(height);
  pinfo->BitsPerPixel = 24;   // Most Simple
  pinfo->CompressionMeth = 0;  // BI_RGB, explicit so it's not overlooked
  // resolutions are 0
  // ColorsInPalette 0
  // NImportantColors 0
  return p; //good luck!
 }


FULLBITMAP *makeClassicBitmap (int width, int height ) 
{
  FULLBITMAP *p = reinterpret_cast<FULLBITMAP *>
    (new uint8_t [ sizeof(CLASSICBMPHEADERS) + 3 * abs(width) * abs(height) ]);
  if( ! p )
    {
      error(1, errno, "memory alloc for full bitmap failed\n\
width=%d, height=%d\n", width, height);
    }
  CLASSICBMPHEADERS *phs = makeClassicHeaders(width, height);
  memcpy(p, phs, sizeof(*phs));
  delete phs;
  return p;
}

void flipClassicBitmap( FULLBITMAP *pbm )
{
  int width  = pbm->headers.infoHeader.Width;
  int height = pbm->headers.infoHeader.Height;
  uint8_t *data = &(pbm->bytes[0]);
  uint8_t *buf = new uint8_t[width*3];

  int ra = 0;
  int rb = height-1;

  while(ra < rb) {
    memcpy(buf, data + width*rb*3, width*3);
    memcpy(data + width*rb*3, data + width*ra*3, width*3);
    memcpy(data + width*ra*3, buf, width*3);
    ra++;  rb--;
  }

  delete[] buf;
  return;
}

bool FULLBITMAP::drawRect( uint16_t xmin, uint16_t rwidth,
			  uint16_t ymin, uint16_t rheight,
			  uint8_t r, uint8_t g, uint8_t b )
{ //C++ vs Python reminder in C++, instead of this.classmemname
  // we code this->classmemname or classmemname
  uint16_t width  = headers.infoHeader.Width;
  uint16_t height = headers.infoHeader.Height;
  if( xmin+rwidth > width ||
      ymin+rheight > height )
    {
      cerr << "FULLBITMAP::drawRect bad dims." << endl;
      cerr << "Image size:";
      fprintf(stderr, "width=%u, height=%u\n", width, height);
      cerr << "Upper Left Corner:";
      fprintf(stderr, "(%u,%d)\n", xmin, ymin);
      cerr << "           Lower Right Corner:";
      fprintf(stderr, "(%u,%u).\n", xmin+rwidth, ymin+rheight);
      return false;
    }

  uint8_t *pdata = & bytes[0];
  for( int x = xmin; x < xmin+rwidth; x++)
    for( int y = ymin; y < ymin+rheight; y++)
      {
	pdata[y*3*width + x*3 + 2] = r;
	pdata[y*3*width + x*3 + 1] = g;
	pdata[y*3*width + x*3 + 0] = b;
      }

  return true;
}


 
  
  


