#include <stdio.h>
#include <error.h>
#include <errno.h>

class BlobGetter {
  FILE* fp;
  size_t len;
  size_t iread;
  int isopen;
 public:
  BlobGetter(FILE* fp, size_t len)
    : fp(fp), len(len), iread(0), isopen(1)
    {}
  size_t getnext( void * buf)
    {
      return getith(buf, iread + 1);
    }
  size_t getith( void * buf, size_t ith)
  {
    if(! isopen )
      {
	return 0;
      }
    if( ith < iread + 1 )
      {
	error(1, 0, "BlobGetter did read %ld blobs, but cant read blob %ld from the past.",
	      iread, ith);
      }
    while(iread + 1  <= ith)
      {
	size_t ret = fread(buf, len, 1, fp);
	if( ret == 1 ) {
	  iread++;
	  if( iread == ith ) {
	      return 1;
	  }
	  //Continue the while loop
	  //We can't do anything but ask os to read and copy entire blob
	  //even though the caller doesn't want it, since our use case
	  //is on streams, not seekable files.
	}
	else {
	  isopen = 0;
	  return 0;
	}
      }
    error(1,errno,"BlobGetter while loop ended, logic or unanticipated error.");
    return 0;
  }
};


