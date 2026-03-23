#include "bmp.h"
#include <iostream>
#include <iomanip>
#include <error.h>
#include <stdio.h>
using namespace std;
int main(int argc, char **argv)
{
  BITMAP_FILEHEADER *pheader = new BITMAP_FILEHEADER;
  fread(pheader, sizeof(BITMAP_FILEHEADER), 1, stdin);
  cout.write( (const char *) pheader->Signature, sizeof( pheader->Signature ));
  cout << "Compilers Idea of header size=" << (sizeof (BITMAP_FILEHEADER)) << endl;
  cout << "FileSize=" << pheader->FileSize << endl
       << "Reserved1=" << pheader->Reserved1 << endl
       << "Reserved2=" << pheader->Reserved2 << endl
       << "BitsOffset=" << pheader->OffsetToData << endl;

  int bmhsize = (pheader->OffsetToData - sizeof(*pheader));

  cout << "Bitmap info struct size="
       << bmhsize << endl;

  BITMAPINFOHEADER *pbmi40  = NULL;
  BITMAPV5HEADER   *pbmi124 = NULL;
  void *p = new uint8_t[bmhsize];
  
  switch (bmhsize) {
  case sizeof (BITMAPINFOHEADER):
    fread(p, sizeof(BITMAPINFOHEADER), 1, stdin);
    pbmi40 = (BITMAPINFOHEADER *) p;
    break;
  case sizeof (BITMAPV5HEADER):
    fread(p, sizeof(BITMAPV5HEADER), 1, stdin);
    pbmi40  =  (BITMAPINFOHEADER *) p;
    pbmi124 =  (BITMAPV5HEADER *) p;
    break;
  default:
    error(1, 0, "Unsupported bitmap header size %d", bmhsize);
  }

  cout << "BMHeader Size=" << pbmi40->MySize << endl;
  cout << "Width="     << pbmi40->Width  << endl;
  cout << "Height="    << pbmi40->Height << endl;
  cout << "#ColorPlanes(should be 1)=" << pbmi40->NColorPlanes << endl;
  cout << "Compression Meth=" << pbmi40->CompressionMeth << endl;
  cout << "( 3 is BI_BITFIELDS, and masks are used ONLY in this case )" << endl;
  cout << "ResolutionHoriz="  << pbmi40->ResolutionHoriz
       << " Vert="            << pbmi40->ResolutionVert << endl;
  cout << "#Palette Colors="  << pbmi40->ColorsInPalette
       << "  #Important ones="  << pbmi40->NImportantColors;

  if(pbmi124) {
    cout << "\nHigher Version Attributes:" << endl;



    cout << "Red Mask=";
    printf("%#010x",pbmi124->bV5RedMask);
    cout << " Green Mask=";
    printf("%#010x",pbmi124->bV5GreenMask);
    cout << " Blue Mask=";
    printf("%#010x",pbmi124->bV5BlueMask);
    cout << "  Alpha Mask=";
    printf("%#010x",pbmi124->bV5AlphaMask);
    cout << endl;

    printf("CSType= %#010x\n", pbmi124->bV5CSType);

    cout << "CIEXYZTRIPLE Endpoints:" << hex << endl;
    cout << "ciexyzRed:" << endl;
    cout << "("   << pbmi124->bV5Endpoints.ciexyzRed.ciexyzX
	 << ", "  << pbmi124->bV5Endpoints.ciexyzRed.ciexyzY
	 << ", "  << pbmi124->bV5Endpoints.ciexyzRed.ciexyzZ << ")" << endl;
    cout << "ciexyzGreen:" << endl;
    cout << "("   << pbmi124->bV5Endpoints.ciexyzGreen.ciexyzX
	 << ", "  << pbmi124->bV5Endpoints.ciexyzGreen.ciexyzY
	 << ", "  << pbmi124->bV5Endpoints.ciexyzGreen.ciexyzZ << ")" << endl;
    cout << "ciexyzBlue:" << endl;
    cout << "("   << pbmi124->bV5Endpoints.ciexyzBlue.ciexyzX
	 << ", "  << pbmi124->bV5Endpoints.ciexyzBlue.ciexyzY
	 << ", "  << pbmi124->bV5Endpoints.ciexyzBlue.ciexyzZ << ")" << endl;
    cout << dec << "Gammas: Red="   << pbmi124->bV5GammaRed
         <<               " Green=" << pbmi124->bV5GammaGreen
         <<               " Blue="  << pbmi124->bV5GammaBlue << endl;
    cout << "Intent=" <<  pbmi124->bV5Intent << endl;
    cout << "Profile Data=" << hex << pbmi124->bV5ProfileData
         << " Profile Size=" << dec << pbmi124-> bV5ProfileSize << endl;
    cout << "Reserved=" << hex << pbmi124-> bV5Reserved << endl;
  } //end of V5 additions

  return 0;
}
