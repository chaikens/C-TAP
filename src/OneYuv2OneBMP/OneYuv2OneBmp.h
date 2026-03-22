#if defined __cplusplus
#include <cstdint>
#else
#include <stdint.h>
#endif
/*
 * gives us uint8_t uint16_t for our image and video files
 */


int OneYuv2OneBmp(unsigned int width, unsigned int height,
		  const uint8_t *YUVin, uint8_t *BMPout);

/* YUVin points to the YUV data in memory.
   BMPout points to memory already allocated for the conversion a bmp file,
   including the 54 byte header */

  
