#include "bmp.h"
#include <iostream>
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

	      
  return 0;
}
