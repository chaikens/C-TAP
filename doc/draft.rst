C-TAP processing sequence
=========================

For one movie

- movie file name MV.<some ext>
  ->(ffmpeg) bitmaps
  ->(1a C++) MV.int
  ->(1b C++) MV.out
  ->(convert from ImageMagik) .bmp's with circles added
  ->(ffmpeg) "baby movie"


- MV.int text file result of analyzing pixel value differences at common coordinate.
  
  example::

    1 34 1908 802 34 1908 802 34 1908 802 -30 1247 858 -30
    1247 858 -30 1247 858 4 4 4 7 7 7 21

    2 38 1800 872 38 1800 872 38 1800 872 -28 1682 790 -28 1682 790 -28
    1682 790 8 8 8 4 4 4 24

    3 31 43 522 31 43 522 31 43 522 -32 1005 370 -32 1005 370 -32 1005 370
    6 6 6 4 4 4 18

  shown grouped and explained::

    +---------------+--------------------------------------------------+
    | 4             | difference frame number from Frame_i-Frame_(i-1) |
    +---------------+--------------------------------------------------+
    | 34 721 676    | max pos differences                              |
    | 34 721 676    | for each color in rgb order,                     |
    | 34 721 676    |   (diff value) (x-coord) (y-coord)               |
    +---------------+--------------------------------------------------+
    | -34 1851 744  | min neg differences                              |
    | -34 1851 744  |    ditto                                         |
    | -34 1851 744  |                                                  |
    +---------------+--------------------------------------------------+
    | 4 4 4         | numbers of times a pix diff breaks a threshold   |
    | 5 5 5         | first line positives, 2nd negatives (rgb order)  |
    +------------------------------------------------------------------+
    | 21            | please look and report, important number         |
    +------------------------------------------------------------------+

- MV.out text file Final result (which will be visualized by drawing circles using convert).
  Each line corresponds to a one-frame pair event, as determined by the software.
  
  -   cluster number  (may be repeated, clusters of events are numbered successively)
  -   frame number  (of one event)
  - signed pixel diff with max abs value
  - x, y coords in the frame of the pixel with diffs max abs value
  - naive bayisan classifier score 0 ...  1 => garbage ... real thing

- baby movie (?? name) selected frames with circles added around event locations.
