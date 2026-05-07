echo SOURCED: C-TAP-YOUR-ALALYSIS-example.h.sh

OPT_CamSett="--CamSett-file $(pwd)/CamSett.txt"

#we have to develop so when we specify pipelining here, and include the below
#analysis options, that is what is done.  We'll look at TESTING/TestPipeOpt code
#to do that.

#---W H E N--- pipeline is used, we define this shell fn someway like these:

#for FULL FRAME extraction.  The argument is the input movie
#function ffmpeg_pipe_extract() {
#    ffmpeg -hide_banner -y -i $1 PIPE.yuv &> ffmpeg.log
#    }
#
#     MOVIE_TO_FRAME_DIV=1
#     MOVIE_SCALE_OPTION="--movie-scale 2"
#     PIXPROC_SCALE_OPTION="--pixproc-scale 1"
#     USER_SCALE_OPTION="--user-scale 1"
#     OTHER_OPTIONS="--no-crop --camera-index 1"
#     #In Phase1a C++ program, we wrote an exclusion zone function for DroneShort1,
#     #and also named a camera for it.

#for HALF FRAME, DECIMATED extraction.
ffmpeg_pipe_extract()
{
    ffmpeg  -hide_banner -y -an   \
	    -i $1                 \
            -vf                   \
            "scale=trunc(iw/4)*2:trunc(ih/4)*2,decimate,setpts=N/100/TB" \
	    PIPE.yuv &> ffmpeg.log
}

     MOVIE_TO_FRAME_DIV=2
     MOVIE_SCALE_OPTION="--movie-scale 1"
     PIXPROC_SCALE_OPTION="--pixproc-scale 1"
     USER_SCALE_OPTION="--user-scale 1"
     OTHER_OPTIONS="--no-crop --camera-index 1"
     #In Phase1a C++ program, we wrote an exclusion zone function for DroneShort1,
     #and also named a camera for it.  
     
     #We're refactoring this from TESTING/TestPipelineOPT where
     #we had after defining the above,
     #source DoOneMoviePipeCommon.h.sh
