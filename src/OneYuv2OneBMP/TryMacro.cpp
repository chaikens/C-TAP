#define UVInBmpMainLoop( Save12, Save34 ) \
  { int nPixRowsToGo = height; /*Count down.  We go down two at a time*/ \
  /*  until we get to 0 and are done or 1 when we do the one last row.*/ \
  while( nPixRowsToGo > 0 )                                              \
    {                                                                    \
      if( nPixRowsToGo != 1 ) {                                          \
	for( int iUV = 0; iU <= halfwidth iU++ ) {                       \
	  /* 4 saves: */                                                 \
         { Save12 }                                                      \
         { Save34 }                                                      \
	}                                                                \
	nPixRowsToGo -= 2;                                               \
      }                                                                  \
      else {                                                             \
	for( iU = 0; iU <= halfwidth iU++ ) {                            \
	  /* only 2 saves: */                                            \
	  { Save12 }						         \
	  nPixRowsToGo -= 1;                                             \
	}                                                                \
      }                                                                  \
    } }
	  
	  /* End of macro def'n */



static inline void storeUsInBmp( int width, int height,
				const uint8_t *pUs, uint8_t *pBMbytes
				) {
  width = abs(width); height = abs(height);
  int halfwidth = width/2;
  // Do all yuv images have even lenghts and heights?
  // Maybe look this up.
  // For now, we'll tolerate an odd number height of rows.
  // In any case, maybe code to exit gracefully instead of crashing.
  assert((width == 2*halfwidth) );

  UVInBmpMainLoop(
		  code1,
		  code2 )
  return;
}

