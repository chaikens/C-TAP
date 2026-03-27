// we have bmps with bits offsets 54 and 138..


#include "mustbelittleendian.h"
//only developed for g++ little endian (like Linux PCs)

#if defined __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#endif

typedef struct __attribute__((packed)) BITMAP_FILEHEADER {
  unsigned char Signature[2];
  uint32_t FileSize;
  uint16_t Reserved1;
  uint16_t Reserved2;
  uint32_t OffsetToData;
} BITMAP_FILEHEADER;

//
// Wikipedia says there are 7 kinds of DIB, Bitmap information headers
// also, they all have 32 bit fields.
// the 40 byte one is BITMAPINFOHEADER
//
// Versions after BITMAPINFOHEADER only add fields to the end of the header of the previous version.
//

typedef struct __attribute__((packed)) BITMAPINFOHEADER {
  int32_t MySize;            /* Wikipedia says its signed. */
  int32_t Width;
  int32_t Height;
  uint16_t NColorPlanes;     /* Wikipedia says must be 1 */
  uint16_t BitsPerPixel;     /* aka Depth */
  uint32_t CompressionMeth;  /* 0=BI_RGB none, most common */
  uint32_t SizeOfRawBmpData; /* could be 0 for BI_RGB */
  int32_t ResolutionHoriz;   /* pixels/meter */
  int32_t ResolutionVert;
  uint32_t ColorsInPalette;  /* 0 to default to 2^n */
  uint32_t NImportantColors; /* Usually ignored, 0 if all colors are used. */
} BITMAPINFOHEADER;

typedef struct __attribute__((packed)) CLASSICBMPHEADERS {
  BITMAP_FILEHEADER fileHeader;
  BITMAPINFOHEADER infoHeader;
} CLASSICBMPHEADERS;

CLASSICBMPHEADERS *makeClassicHeaders ( int width, int height );
// given width and height, allocates and completely sets a 
// classic bitmap header (with 40-byte BITMAPINFOHEADER).
// Returns a pointer P to it.
// The storage is given to the caller; Deallocate it to avoid mem leaks with
// delete P;

typedef struct __attribute__((packed)) FULLBITMAP {
  const CLASSICBMPHEADERS headers;
  uint8_t bytes[];
} FULLBITMAP;


FULLBITMAP *makeClassicBitmap (int width, int height );
// given width and height, allocates and completely sets a 
// classic bitmap (with 40-byte BITMAPINFOHEADER) for 0 (all Black) image.
// Returns a pointer P to it.
// The headers are declared constant since there is no need to modify them.
// The storage is given to the caller; Deallocate it to avoid mem leaks with
// delete P;

void flipClassicBitmap( FULLBITMAP *pbm );
// Handy, temporary solution to upside-down results.
// It's more efficient to store the rows in the right place when generating them!


// Wikipedia says there are 7 kinds of DIB, Bitmap information headers
// also, they all have 32 bit fields.
// the 40 byte one is BITMAPINFOHEADER
//
// Versions after BITMAPINFOHEADER only add fields to the end of the header of the previous version.
//


//
// 138-14=124, so the other header we have is BITMAPV5HEADER (the biggest!)
// wikip says it's written by the GIMP!
//
// https://learn.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapv5header
// https://learn.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapinfoheader
/*
typedef struct tagBITMAPINFOHEADER {
  uint32_t biSize;
  int32_t  biWidth;
  int32_t  biHeight;
  uint16_t  biPlanes;
  uint16_t  biBitCount;
  uint32_t biCompression;
  uint32_t biSizeImage;
  int32_t  biXPelsPerMeter;
  int32_t  biYPelsPerMeter;
  uint32_t biClrUsed;
  uint32_t biClrImportant;
} BITMAPINFOHEADER, *LPBITMAPINFOHEADER, *PBITMAPINFOHEADER;
*/

// from
// https://stackoverflow.com/questions/20864752/how-is-defined-the-data-type-fxpt2dot30-in-the-bmp-file-structure
//[...] FXPT2DOT30 [...] which means that they are
// interpreted as fixed-point values with a 2-bit integer part and a 30-bit fractional part.

//
// Microsoft's user.h (under our doc/Resources/) defines FXPT2DOT30 as LONG, so:
//
typedef int32_t FXPT2DOT30;

//from https://learn.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-ciexyz
typedef struct CIEXYZ {
  FXPT2DOT30 ciexyzX;
  FXPT2DOT30 ciexyzY;
  FXPT2DOT30 ciexyzZ;
} CIEXYZ;


typedef struct CIEXYZTRIPLE {
  CIEXYZ ciexyzRed;
  CIEXYZ ciexyzGreen;
  CIEXYZ ciexyzBlue;
} CIEXYZTRIPLE;

typedef struct __attribute__((packed)) BITMAPV5HEADER {
  uint32_t        bV5Size;
  int32_t         bV5Width;
  int32_t         bV5Height;
  uint16_t        bV5Planes;
  uint16_t        bV5BitCount;
  uint32_t        bV5Compression;
  uint32_t        bV5SizeImage;
  int32_t         bV5XPelsPerMeter;
  int32_t         bV5YPelsPerMeter;
  uint32_t        bV5ClrUsed;
  uint32_t        bV5ClrImportant;
  uint32_t        bV5RedMask;
  uint32_t        bV5GreenMask;
  uint32_t        bV5BlueMask;
  uint32_t        bV5AlphaMask;
  uint32_t        bV5CSType;
  CIEXYZTRIPLE    bV5Endpoints;
  uint32_t        bV5GammaRed;
  uint32_t        bV5GammaGreen;
  uint32_t        bV5GammaBlue;
  uint32_t        bV5Intent;
  uint32_t        bV5ProfileData;
  uint32_t        bV5ProfileSize;
  uint32_t        bV5Reserved;
} BITMAPV5HEADER;
