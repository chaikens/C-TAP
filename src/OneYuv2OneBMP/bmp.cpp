#include "bmp.h"

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
  pfh->FileSize = sizeof(CLASSICBMPHEADERS) + 3*width*height;
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
  
  





  return p;
}


