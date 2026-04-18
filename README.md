# C-TAP
# C-TAP
You're on the devel-pipeline branch.

To try this out, get the 125Mb movie named DroneShort1.mp4 from
Google drive folder https://drive.google.com/drive/folders/15EOdKlvIPG9ORDVy3WBh5rX8cFSmmVQ7 .  
If you don't have access, contact Matt Szydagis.

Unless the current commit (see log) is faulty, you can try two things.
First, put DroneShort1.mp4 in  this directory

(1) ./C-TAP.sh (should reproduce what happens in v1.0)
You will find the results in the RESULTS directory.

(2) ./C-TAPPipe.sh should do what we currently have working 
in deploying more and more pipelining.  We are also doing
refactorings, such as coding separate steps in separate 
C++ programs or Bash scripts.

The other movies in that Google drive folder are (1) for very short
tests and (2) testing the functionality automatically iterating over
a collection of movies.

v1.0 (in the main branch) is mild refactorization of the original C-TAP. 
It puts .bmp (Classical, 54-byte header) files on the disk after using 
ffmpeg to extract non-redundent frames for a movie, for subsequent 
processing.  In progress is v2.0 which
will avoid using disk space for this by pipelining.  We try to 
retain v1.0 functionality in devel-pipeline for regression
tests, performance analysis, etc.

