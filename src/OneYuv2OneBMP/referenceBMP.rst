Please render this as code::

4096x4096 = (2^12)*(2^12) = (2^8)*(2^8)*(2*8) = 16777216 = 0x1000000
pixels, one for each 8-bit GRB value.

Correponding YUV I420p frame
4096*4096 Y-bytes    16777216   0x1000000 2^24
4096*4096/4 U bytes   4194304     x400000 2^22
4096*4096/4 V bytes   4194304     x400000 2^22
2*4194304 =           8388608     x800000 2^23
16777216+8388608=25165824 = 4096*4096*(3/2) size of YUV file   
Definition of the YUV values for each pixel in the 4096x4096 Universal Image.
The *'s represent pixels.  For each pixel in the array, the YUV values for it are given
by the horizontal and vertical labels.  Each contiguous 16x16 square begins at even
pixel coordinates, all even aligned 2x2 subsquares get their U or V values from
one entry in the U array or V array.

            |           | |           |         |           |
            |----U=0----| |----U=1----|   ...   |---U=255---|
            |           | |           |         |           |
            0 1 Ylo 14 15 0 1 Ylo 14 15         0 1 Ylo 14 15
	 
 ---     0  * * ...  *  * * * ...  *  *   ...   * *  ... *  *
 |       1  * * ...  *  * * * ...  *  *   ...   * *  ... *  *
V=0    Yhi 
 |      14  * * ...  *  * * * ...  *  *   ...   * *  ... *  *
 ---    15  * * ...  *  * * * ...  *  *   ...   * *  ... *  *
 ---     0  * * ...  *  * * * ...  *  *   ...   * *  ... *  *
 |       1  * * ...  *  * * * ...  *  *   ...   * *  ... *  *
V=1    Yhi   .  ...           ...         ...        ...    .
 |      14  * * ...  *  * * * ...  *  *   ...   * *  ... *  *
 ---    15  * * ...  *  * * * ...  *  *   ...   * *  ... *  *
 ---

 ...        .   ...            ......                ...
    
 ---
 ---    0   * * ...  *  * * * ...  *  *   ...   * *  ... *  *
  |     1   * * ...  *  * * * ...  *  *   ...   * *  ... *  *
V=255  Yhi      ...           ...         ...        ...
  |    14   * * ...  *  * * * ...  *  *   ...   * *  ... *  *
 ---   15   * * ...  *  * * * ...  *  *   ...   * *  ... *  *

 (256*16)*(256*16) = 2^24 = 16777216 = size of YUV file.

 
