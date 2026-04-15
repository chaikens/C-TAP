/*
Since this is for a first step in making 1a not rely on .bmp disk files,
we will hand the task off to ffmpeg via files.

If we continue to run 1a on bmp data, a more efficient soln may be better.
 */


#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <error.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "OneYuv2OneBmp.h"

int OneYuv2OneBmp(unsigned int width, unsigned int height,
		     const char *YUVin, char *BMPout)
{
  system("rm -f OneTemp.yuv OneTemp.bmp");
  //printf("Will open OneTemp.yuv\n");
  int yuvFD = open("OneTemp.yuv", O_CREAT | O_RDWR, S_IRUSR|S_IWUSR);
  if( yuvFD < 0 )
    {
      error(1, errno, "Open OneTemp.yuv failed.");
    }
  size_t yuvlen = ( width * height * 3 ) / 2;
  size_t yuvwriteret = write(yuvFD, YUVin, yuvlen);

  /* construct ffmpeg call like
ffmpeg -hide_banner -an -video_size 3840x2160 -i OneTemp.yuv -frames:v 1 OneTemp.bmp > ffmpeg.messages 2>&1 
  Only the size is variable. 
  */
  char command[240];
  command[0] = 0;
  const char *comPrefix = "ffmpeg -hide_banner -an -video_size ";
  strncat(command, comPrefix, strlen(comPrefix));
  char vidsizestr[12];
  int sizelen = sprintf(vidsizestr, "%ux%u", width, height);
  strncat(command, vidsizestr, sizelen);
  const char *comSuffix = " -i OneTemp.yuv -frames:v 1 OneTemp.bmp > OneYuvs.ffmpeg.msgs 2>&1";
  /* bash &> for redir both stdout and stderr didn't work */
  strncat(command, comSuffix, strlen(comSuffix));

  system(command);

  int bmpFD = open("OneTemp.bmp", O_RDONLY, 0);
  size_t readcount= 0;
  while ( readcount < yuvlen*2+54 )
    {
      //printf("of OneTemp.bmp, we did read %ld bytes.\n", readcount);
      size_t bmpreadret = read(bmpFD, BMPout, yuvlen*2+54);
      if( bmpreadret < 0 )
	{
	  error(1, errno, "Read of OneTemp.bmp failed.");
	}
      readcount += bmpreadret;
    }
  system("rm -f OneTemp.bmp OneTemp.yuv");
  close(bmpFD);
  close(yuvFD);
  return 0;
}


