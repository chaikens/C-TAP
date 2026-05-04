# C-TAP Intro
Please check out Section 3.2.1 on image analysis for UAP research starting on page 11 of https://arxiv.org/pdf/2312.00558
# C-TAP
You're on the devel-pipeline branch.
=======
To try this out, get the 125Mb movie named DroneShort1.mp4 from
Google drive folder https://drive.google.com/drive/folders/15EOdKlvIPG9ORDVy3WBh5rX8cFSmmVQ7 .  
If you don't have access, contact Matthew Szydagis at <matthew.szydagis@gmail.com>.

Unless the current commit (see log) is faulty, you can try two things.
First, put DroneShort1.mp4 in  this directory

(1) ./C-TAP.sh (should reproduce what happens in v1.0)
You will find the results in the RESULTS directory.

(2) ./C-TAPPipe.sh should do what we currently have working 
in deploying more and more pipelining.  We are also doing
refactorings, such as coding separate steps in separate 
C++ programs or Bash scripts.
=======
Put DroneShort1.mp4 in  this directory, and run
./C-TAP-DroneShort1HalfDecimated.sh
You will find the results in the RESULTS directory.
Our results for two test cases (except for the .MOV movies)
are in C-TAP/RESULTS and C-TAP/RESULTS-GOOD-OLD.
The latter is for the regression testing scripts.

If you run this successfully, you'll find
RESULTS/DroneShort1HalfDecimated.MOV which
the drone discovered and tracked by the software.
>>>>>>> devel-scaling

The other movies in that Google drive folder are (1) for very short
tests and (2) testing the functionality automatically iterating over
a collection of movies.

<<<<<<< HEAD
v1.0 (in the main branch) is mild refactorization of the original C-TAP. 
It puts .bmp (Classical, 54-byte header) files on the disk after using 
ffmpeg to extract non-redundent frames for a movie, for subsequent 
processing.  In progress is v2.0 which
will avoid using disk space for this by pipelining.  We try to 
retain v1.0 functionality in devel-pipeline for regression
tests, performance analysis, etc.
=======
v1.1, has usability and logging improvements,
enables one to select a large capacity and fast destination
for the many one .bmp file per movie frame, and includes
the beginning of developing support for clipping in terms
of arbitrary image resolutions instead of 1920x1040 only.
That is in progress on the devel-scaling branch.

