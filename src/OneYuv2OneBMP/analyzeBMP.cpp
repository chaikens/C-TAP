#include "bmp.h"
#include <iostream>
#include <iomanip>
#include <error.h>
#include <stdio.h>
using namespace std;
int main(int argc, char **argv)
{
  if(argc != 1) {
    error(1, 0,
	  "Please run me with no arguments and input redirected like < file.bmp.\n");
  }
  BITMAP_FILEHEADER *pheader = new BITMAP_FILEHEADER;
  fread(pheader, sizeof(BITMAP_FILEHEADER), 1, stdin);
  cout << "Signature=";
  cout.write( (const char *) pheader->Signature, sizeof( pheader->Signature ));
  cout << endl;
  cout << "Compiler'ss Idea of header size=" << (sizeof (BITMAP_FILEHEADER)) << endl;
  cout << endl;
  cout << "FileSize=" << pheader->FileSize << endl
       << "Reserved1=" << pheader->Reserved1 << endl
       << "Reserved2=" << pheader->Reserved2 << endl
       << "BitsOffset=" << pheader->OffsetToData << endl;

  int bmhsize = (pheader->OffsetToData - sizeof(*pheader));

  cout << "(calculated) Bitmap info struct size="
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

  cout << endl;
  cout << "BMHeader Size=" << pbmi40->MySize << endl;
  cout << "Width="     << pbmi40->Width  << endl;
  cout << "Height="    << pbmi40->Height << endl;
  cout << "#ColorPlanes(should be 1)=" << pbmi40->NColorPlanes << endl;
  cout << "Compression Meth=" << pbmi40->CompressionMeth << endl;
  if(pbmi40->CompressionMeth == 3) {
    cout << "( 3 is BI_BITFIELDS, and masks are used ONLY in this case )"
	 << endl;
  }
  else {
    cout << " (Masks for pixel format are not used, they're only for case 3.)"
	 << endl;
  }
  cout << "ResolutionHoriz="  << pbmi40->ResolutionHoriz
       << " Vert="            << pbmi40->ResolutionVert << endl;
  cout << "#Palette Colors="  << pbmi40->ColorsInPalette
       << "  #Important ones="  << pbmi40->NImportantColors << endl;

  if(pbmi124) {
    cout << endl << "\nHigher Version Attributes:" << endl;

    uint8_t *pbyte;
    cout << " Red   Mask=";
    printf("%#010x",pbmi124->bV5RedMask);

    pbyte = (uint8_t *) &(pbmi124->bV5RedMask);
    printf(" bytes %#04x %#04x %#04x %#04x\n",
	   (uint8_t) pbyte[0],
	   (uint8_t) pbyte[1],
	   (uint8_t) pbyte[2],
    	   (uint8_t) pbyte[3]);

    cout << " Green Mask=";
    printf("%#010x",pbmi124->bV5GreenMask);

    pbyte = (uint8_t *) &(pbmi124->bV5GreenMask);
    printf(" bytes %#04x %#04x %#04x %#04x\n",
	   (uint8_t) pbyte[0],
	   (uint8_t) pbyte[1],
	   (uint8_t) pbyte[2],
    	   (uint8_t) pbyte[3]);

    cout << " Blue  Mask=";
    printf("%#010x",pbmi124->bV5BlueMask);

    pbyte = (uint8_t *) &(pbmi124->bV5BlueMask);
    printf(" bytes %#04x %#04x %#04x %#04x\n",
	   (uint8_t) pbyte[0],
	   (uint8_t) pbyte[1],
	   (uint8_t) pbyte[2],
    	   (uint8_t) pbyte[3]);

    cout << " Alpha Mask=";
    printf("%#010x",pbmi124->bV5AlphaMask);

    pbyte = (uint8_t *) &(pbmi124->bV5AlphaMask);
    printf(" bytes %#04x %#04x %#04x %#04x\n",
	   (uint8_t) pbyte[0],
	   (uint8_t) pbyte[1],
	   (uint8_t) pbyte[2],
    	   (uint8_t) pbyte[3]);

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

  uint8_t *pp = new uint8_t[4];
  fread(pp, 1, 4, stdin);
  /*OK if we dont read them all for a 1-pixel 0 compressed bmp.*/
  cout << endl << "First Pixel, memory order:" << endl;
  printf("%d %d %d",
	 (unsigned char) pp[0],
	 (unsigned char) pp[1],
	 (unsigned char) pp[2]);
  switch (pbmi40->CompressionMeth) {
  case 0 : printf("\n"); break;
  case 3:  printf(" %d\n",
		  (unsigned char) pp[3]);
    printf("See Masks above.\n");
    break;
  default: printf(" <- first 3 bytes. Unsupported compression method %u\n",
		  pbmi40->CompressionMeth);
  }
  
  
  return 0;
}
