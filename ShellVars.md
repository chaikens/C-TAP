Def in C-TAP-jobname.sh 

REUSE_BMPS
FAST_FILESYS_DIR_IF_USED
DEBUG
BITMAPS_PARENT_DIR
NEW_BITMAPS
NEW_CamSett
BITMAPS_DIR_NAME
BITMAPS_PARENT_DIR 
(we cd to it so ffmpeg extraction put bmps there,
could be a parameter of the ffmpeg command like 
/home/me/dodo/bitmaps/thumb%d0000000.bmp)
BITMAPS_DIR
NEW_bitmaps
NEW_Camsett
SLOW_MOVIE_DIR

ext

movie_files

moviePrefix (set by script from first movie_file)
Will different movies in a given dir have different prefixes?

SOFTWARE_DIR
RESULTS_DIR

(result patterns .in, .out .MOV)

Phase1a, Phase1b (set to Names of programs)

SLOW_FILESYS_REPORT

COPYMOVIES

numMovieFiles

movie_file (shell loop variable)

o (shell loop variable for 1, 2, 3 ... )

FAST_MOVIE_DELETE_ME

FileName

COMMAND_ARCHIVE_PATHNAME 

ALL_MESSAGES_PATH

FFMPEG_EXTRACT_CMD (replaced by shell fun() in devel-pipeline..TestPipeOpt)
(shell can print shell funs, for our log, with "type functionName")


nframes


Beginning of Phase1a
RESULT_OF_1a_BASE
Phase1a_cmd_args (builds command)
Phase1a_cmd (what's run)
err

u, v for use Phase1a stages, I think unnecessary since now
mem use of Phase1a doesn't expand with number of frames.  Now unused.

ndiffs (should be nframes-1)

Beginning of Phase1b

RESULT_OF_1b_BASE (empty file made by us)

Phase1b_xterm_PID (so we see .out file)
Phase1b
Phase1b_cmd
Phase1b_mess_xterm_PID (so we see messages)

n (wc is used to count result lines)

Phase1b_redo_command, Phase1b_redo_command_args

Begin baby movie .MOV (quicktime, old variant of mp4) making.

vars for reading .out lines: evt frame extr x y prob
i,j,k,l = x,y +/- 9 (circle size changes w/ scale!)

BITMAP_EDIT_CMD












C-TAP-HELPERS defines
#Phase1a reads depth 24 (8x8x8) .bmps only
function depthOfBmpIs24()
function widthOfBmp()
function heightOfBmp()
function widthOfMovie()
function heightOfMovie()
function exit_greeting() {optionally kills xterms by PID}




