# C-TAP Intro
Please check out Section 3.2.1 on image analysis for UAP research starting on page 11 of https://arxiv.org/pdf/2312.00558
# C-TAP
You're on the devel-pipeline branch.
=======
To try this out, get the 125Mb movie named DroneShort1.mp4 from
Google drive folder https://drive.google.com/drive/folders/15EOdKlvIPG9ORDVy3WBh5rX8cFSmmVQ7 .  
If you don't have access, contact Matthew Szydagis at <matthew.szydagis@gmail.com>.

The results of today's 4-way comparison are visible in the 4 directories
RESULTS-jobDS1[Full,Half]Decimated[FRAME,PIPE].

Try it yourself:
First, put DroneShort1.mp4 in  this directory.

Then, edit directory entries in
the 4 C-TAP-jobDS1*.sh files for your system.

Finally, execute DEMO.sh
A shell run of all 4 is in DEMO.shellrun.txt

The results and logs for the 4 demo runs, except for the .MOV baby movies, are in the
directories symbolically linked from DemoFullFrame DemoFullPipe DemoHalfFrame DemoHalfPipe.

We now have pipelining and .bmp frame storage as options to the same system. The results 
show that for this test movie, the (1) .bmp disk storage architecture and (2) architecture
to pipe extract raw YUV images through our table-based YUV-BMP converter to the first
phase of the FLIR algorithm 
images yield identical results. This is true even though there are small differences
between the .bmps produced by ffmpeg and our converter.

The other movies in that Google drive folder are (1) for very short
tests and (2) testing the functionality automatically iterating over
a collection of movies.

 and improved configuration
(CamSett.txt) management, to demonstrate the cropping functionality.

v1.0 (in the main branch) is mild refactorization of the original C-TAP. 
It puts .bmp (Classical, 54-byte header) files on the disk after using 
ffmpeg to extract non-redundent frames for a movie, for subsequent 
processing.  In progress is v2.0 which
will avoid using disk space for this by pipelining.  We try to 
retain v1.0 functionality in devel-pipeline for regression
tests, performance analysis, etc.

v1.1, has usability and logging improvements,
enables one to select a large capacity and fast destination
for the many one .bmp file per movie frame, and includes
the beginning of developing support for clipping in terms
of arbitrary image resolutions instead of 1920x1040 only.
That is in progress on the devel-scaling branch.

Developed on Ubuntu. 
Packages required for just the FLIR algorithm:
git
gcc
g++
ffmpeg
imagemagick
xterm

Optional:
YUView (to display the yuv 420p to bgr conversion table we build)

