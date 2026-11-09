    * `C-TAP-<jobname>.sh` Top level script

    * `Demo.sh` DroneShort1.mp4 (short) demo invocations of `C-TAP-<jobname>.sh` 

    * `FrameExtractProcess/` 

    * `src/`

    * `PP/`
		Preprocess: with ffmpeg on input movie <name>.mp4, draw orig frame numbers, reduce resolution, decimate, and write to PP-<name>.mp4.  Decimation is an FLIR algorithm step necessary when moving 
		object speeds are too low.

		`PPOneMovie.sh` Edit to specify ffmpeg call details.
		`PPDir.sh`  
	
    * `Graphics/`
	
    * `Visualization`
	
    * `m/`
	
    * `CamSett/`
	
    * `doc/` 

    * `BitA.h` class to allocate a zero rectangular bit array, set one bit at a time,  write to a named file. 
	
	
