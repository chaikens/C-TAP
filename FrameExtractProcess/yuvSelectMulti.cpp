/**
 yuvSelectMulti WidthxHeight framenums-fd yuvinput-fd [at least one option]

 Width, Height must be even.  
 fd's are small integers. Don't use 2 since stderr is used for verbose msgs and errors.
 (Typically, you choose and redirect a pipe or a file to them.)
 Frame numbering begins with 1.
 2nd input is a whitespaced list of strictly increasing frame numbers you want.
 3nd input is the sequence all of yuv frames, an I420p raw encoded movie.

There are 3 use cases specified by mutually exclusive options and their suboptions:

--yuv-out-fd <fd>  Outputs stream of yuv frames through file descriptor fd

--bmp-out-fd <fd> Outputs stream of 54-B header bmp frames through file descriptor fd
         [--bmp-conversion kind(TBD)]

--bmp-out-dirpath <writable dir name for .bmp files> [--bmp-conversion kind(TBD)]
         [--bmp-prefix myname  Written files are named mynamedddddd.bmp, 
                               not FRAMEdddddd.bmp]

optional options:

--compress[="cmd [options]]"] Only for --bmp-out-dirpath.   
         Writes the gzip compressed version of each .bmp.
         Any compression command possibly with options can be given as an option; must use
         ="..." option format. The file name of the .bmp to be compressed will be the last 
         argument to the compression cmd.

--verbose
--seline-fmt '..%d..' sscanf format for extracting one int from one frame selection data line,
   It must have a conversion specification for EVERY field in each line, so fscanf will scan over
   the whole line before the next line.  Thus %*<char> should be used to skip fields.
   EG. Use default '%d' for just frame numbers; 
    you can do --seline-fmt '%*d %d %*d %*d %*d %*f' for C-TAP .out file lines.
--offset noff
   number noff is added to each frame number to obtain which
   frame is taken.  (So if noff=1, the first input frame is skipped.)
   It will cause a warning and extract fewer frames than requested.
   When frames are stored in a directory, their names will bear their original numbers.
      

  This program doesn't look inside yuv frames except to convert them to .bmps,
  but assumes their length is (3*width*height)/2, which is for I420p format.
 (So you can can psychedelic fun with random input.)
*/
#include <string>
#include <stdio.h>
#include <error.h>
#include <errno.h>
#include <iostream>
#include <getopt.h>
#include <stdint.h> //for bytes=uint8_t
#include "bmp.h"
#include "yuvtobmpT.h"
#include <cassert>
#include <unistd.h> //for syscalls like read(), fork, exec
#include <sys/wait.h>
#include <sys/types.h> //for pid_t ret from wait
#include <string.h> //for str_tok_r, old war horse.
#include <vector>
using namespace std;

static const char usage[] = "yuvSelectMulti WidthxHeight framenums-fd yuvinput-fd [one --option required]\n\
-fd's are small integers.\n";

//common data from command line arguments, since raw files don't have metadata
//we ONLY support I420p ones.
static char *cmd;  //will =argv[0], for messages.

static  unsigned int width, height;

//common required fds from command line arguments
static int fnsfd = -1;
static int yuvinfd = -1;

//options
static int vb = 0;                     //verbose
static int do_yuv_stream_out = 0;
static int yuvoutfd = -1;
static int do_bmp_stream_out = 0;
static int bmpoutfd = -1;
static int do_bmp_dir_out = 0;
static char *bmpdirpath = 0;           //used if do_bmp_dir_out
static string bmpdirpaths = "";
static string bmpnameprefixs = "thumb";
static string bmpconversions = ""; //no choices yet
static const char *selinefmt = "%d";  //to parse a file of frame numbers.
static int offset = 0;

//for compressing .bmp files in their directory (by forked subprocesses)
static int do_cx  = 0;
const static char* cx_prog = "gzip";

//Default uses no options.
//[0]=progname [1]=filename (fill in)
//[2]=0 to terminate argv for execvl.
vector<char*> cx_argv = {(char*)cx_prog, 0, 0};
vector<char*> mk_cx_argv(char *s)
{
  cx_argv.clear();
  char *saveptr;
  char *nxt = strtok_r(s, " ", &saveptr);
  while( nxt ) {
    //Put in program name and then any options
    cx_argv.push_back( nxt );
    nxt = strtok_r(0," ",&saveptr);
  }
  //Make the entry to fit in one char* filename
  cx_argv.push_back( 0 );
  //Finally, make the entry containing 0 to
  //terminate the argv passed as execvl 2nd arg.
  //Note it has the pgm name in [0], while
  //we also pass the executable file name. 
  cx_argv.push_back( 0 );
  return cx_argv;
  //To change the file name:
  // cx_argv.data( )[cx_argv.size()-2] = (char *) "Nextfile.bmp"
  //
  //Note:  This code can be easily used to generate a command
  //for compressing the many files listed by separated arguments
  //after the compression program and its options.  
}

static int cx_maxkids = 8;  //to run concurrent compressions.

//FILE POINTERS
static FILE *fnsFP = 0;    //required, frame numbers wanted
static FILE *yuvinFP = 0;  //required, yuv frame input stream
static FILE *yuvoutFP = 0; //optional, yuv frame output stream
static FILE *bmpoutFP = 0; //optional, bmp frame output stream

static class BMclass *pBM; //for when we make bmps

static int get_our_options( int *argc, char **argv[]);
static char *padnumto6( unsigned int n)
{
  static char buf[7];
  sprintf(buf, "%06u", n); //it puts null in buf[6]
  return buf;
}

static void optionprocess() {
  int sum = do_yuv_stream_out + do_bmp_stream_out + do_bmp_dir_out;
  if ( sum != 1) {
    error(1, 0, "Exactly one output option must be given, not %d.", sum);
  }

  if( do_yuv_stream_out ) {
    yuvoutFP = fdopen(yuvoutfd, "w");
    if ( ! yuvoutFP )
      {
	error(1,errno, "Cant open fd %d for writing as a FILE.", yuvoutfd);
      }
  }

  if( do_bmp_stream_out || do_bmp_dir_out )
    {
      if(vb) { cerr << "Making a BMclass width=" << width << " height=" << height << endl; }
      pBM = new BMclass(width, height);
    }
      
  if( do_bmp_stream_out ) {
    bmpoutFP = fdopen(bmpoutfd, "w");
    if ( ! bmpoutFP ) {
      {
	error(1, errno, "Cant open fd %d for writing as a FILE.", bmpoutfd);
      }
    }
  }
  
  if( do_cx && !do_bmp_dir_out ) {
    cerr << "yuvSelectMulti: --compress supported ONLY for --bmp-out-dirpath. "
	 << "Ignored now." << endl;
  }

  //  if( do_cx ) {

  // }
}

  
uint8_t *yuvbuf = 0;

int main(int argc, char *argv[]) {
  cmd = argv[0]; //for messages
  get_our_options(&argc, &argv); //lops off specified --options, just --verbose for now.

  if(vb) { cerr << "Hi. yuvSelectMulti is verbose" << endl; }

  int ret; //let's reuse, ugh.
  if( argc != 4 ) //after --format arg processing, cmd + dims + wantedfd + inputfd
    {
      cerr << endl << "argc=" << argc << endl;
      for (int i = 0; i<argc; i++)
	cerr << "argv[" << i << "]=" << argv[i] << endl;
      cerr << usage << endl << "Will crash now...hope I'm compiled for debugging.. wwwwoooo!!VvvvvBANG." << endl;
      assert(0);
     }
 
  // 1st required arg: WWWWxHHHH 
  if ( 2 != (ret = sscanf(argv[1],"%ux%u", &width, &height)))
    {
      error(1, 0, "First param %s should be WxH, both decimal strings\n%s", argv[1], usage);
    }
  if ( ((width & 1) != 0) || ((height & 1) != 0) )
    {
      error(1, 0, "width %d and height %d must be even (don't know what if not) for us."
	    "\n Though, %s doesn't look inside yuv frames",
	    width, height, argv[0]);
    }
  size_t yuvsize = (width*height*3)/2;
  yuvbuf = new uint8_t[yuvsize];  //Big Mem buffer.
  if (! yuvbuf) {
    error(1, errno, "Cant allocate size %lu buffer.", yuvsize);
  }
  
  // 2nd required arg: frame number file fd
  if( 1 != (ret = sscanf(argv[2], "%d", &fnsfd) ) )
    {
      error(1, 0, "after WWWxDDD must be fd for file or PIPE of wanted frame numbers, not %s", argv[2]);
    }
  fnsFP = fdopen(fnsfd, "r");
  if ( ! fnsFP )
    {
      error(1,errno, "Cant open fd %d for reading fr. numbs. to stdio FILE.", fnsfd);
    }
  
  // 3rd required arg: yuv file or PIPE fd
  if( 1 != (ret = sscanf(argv[3], "%d", &yuvinfd) ) )
    {
      error(1, 0, "after fr. nums fd must be fd PIPE or file to input yuv frames %s", argv[2]);
    }
  yuvinFP = fdopen(yuvinfd, "r");
  if ( ! yuvinFP )
    {
      error(1,errno, "Cant open fd %d for reading yuvs to stdio FILE.", yuvinfd);
    }

  optionprocess(); //Check consistency and do settings that vary with options.

  //Frame numbers will start at 1.
  int fcount = 0;
  int fwanted;
  int gotcount = 0;
  int didreadframe = 0;
  
  if(vb>1) {
    cerr << "do_bmp_stream_out=" << do_bmp_stream_out
         << " do_bmp_dir_out=" << do_bmp_dir_out
         << " do_yuv_stream_out=" << do_yuv_stream_out << endl;
  }
  //Loop to read the next wanted frame number into------V-----
  int nkidsalive=0; //for compression only
  
  while ( (didreadframe = fscanf(fnsFP, selinefmt, &fwanted), didreadframe) == 1  )
    { 
      fwanted = fwanted + offset;
      while ( fwanted >= (fcount+1) )
	{
	  bool fwasread = false; //Don't know.
	  
	  //We have to check for EOF on the input stream by TRYING TO READ!
	  int g = fgetc(yuvinFP);
	  if(vb>1) cerr << cmd << " fgetc got " << g << endl;
	  if( g == EOF ) {
	    cerr << cmd << "end of input stream when frame "
		 << fwanted << "was wanted and "
		 << fcount << " frames were read" << endl;
	    fclose( yuvinFP );
	    //
	    // Do we have to wait for compressing children?
	    return 1;  //Kind of error since the stream ended before a wanted frame.
	  }      
	  int ung = ungetc(g, yuvinFP);//Not at EOF, but 1 char was read; put it back
      
	  if(vb>1) cerr << cmd << " did ungetc got " << ung << endl;
      
	  if( fwanted == (fcount+1) )
	    {
	      //We will love you! But let yuvtobmpT read from the stream if we want a bmp.
	      if( do_bmp_stream_out || do_bmp_dir_out )
		{
		  //Next, read one yuv to convert, then write to stream or a file.
		  //Yes, we have a handy BMclass <- pBM ready to write bgr data into
		  //We'll just fill it and then write it all.  One might write pixel by
		  //pixel, my guess is that's not worth it for performance.
		  
		  if (yuvtobmpT( yuvinFP, pBM ) )  //gets w/h from *pBM,
		    //no erronous return inplemented yet.
		    {
		      cerr << "yuvtobmpT returned error." << endl;
		      error(1, 0, "%s yuvtobmpT returned error.", cmd);
		    }
		  fcount++; gotcount++;  //consumed a yuv and got a bit map we want. 
		  if(vb) cerr << "Did read and made bmp of frame " << fcount << endl;
		  
		  if( do_bmp_stream_out )
		    {
		      if(vb) {
			cerr << "doing do_bmp_stream_out to pBM->write to a FP." << endl;}
		      pBM->write(bmpoutFP);
		    }
		  
		  if( do_bmp_dir_out )
		    {
		      string d = bmpdirpaths
			+ string("/")
			+ bmpnameprefixs
			+ string (padnumto6(fcount))
			+ string (".bmp");
		      if(vb) {cerr << "Trying to write " << d << endl;}
		      const char *s = d.c_str();
		      if(vb) {cerr << "That, in C string form, is " << s << endl;}
		      pBM->write( s );
		      
		      if( do_cx )
			{ //fork off compressor who will write its result to the dir.
			  int kstatus;
			  if(vb) cerr << "We must compress. Frame is " << fcount << endl;
			  if(vb) cerr << "fwanted=" << fwanted << " kidsalive=" << nkidsalive << endl;
			  while (nkidsalive > cx_maxkids)
			    {
			      if(vb) cerr << "Waiting.." << endl;
			      pid_t pidret = wait(&kstatus);
			      nkidsalive--;
			      if(vb>1) {
	  cerr << fflush<< nkidsalive << " kids left, Waitret pid " << pidret << " status " << kstatus << endl << fflush;}
			    }
			  if(vb) cerr << "Big Dad will fork. fwanted=" << fwanted << endl;
			  pid_t pid =fork();
			  if( !pid )
			    {
			      if(vb) cerr << "Hi from kid " << getpid() << " of " << getppid() <<
				       " to compress " << s << endl;
			      cx_argv[cx_argv.size()-2] = (char*) s;
			      execvp(cx_argv[0], cx_argv.data());
			    }
			  else
			    {
			      nkidsalive++;
			    }
			}
		    }
	    
		      if( do_yuv_stream_out )
		    {
		      size_t rret = fread( yuvbuf, yuvsize, 1, yuvinFP );
		      if ( rret != 1 ) {
			cerr << argv[0] << " stops. yuv stream ran only "<< fcount << " frames when frame "
			     << fwanted << " was wanted. ???" << endl;
			return 1;  //out of reading wanted frame number loop
		      }
		      fcount++; //yup, we read a frame.
		      if(vb) { cerr << "Did read frame " << fcount << endl; }
		      if(vb) { cerr << "Try to output frame " << fcount << endl; }
		      size_t wret = fwrite( yuvbuf, yuvsize, 1, yuvoutFP );
		      if( wret != 1 )
			{
			  error( 1, errno, "Failure to write frame %ul to output stream.", fwanted);
			}
		      else
			{
			  if(vb) cerr << "Wrote frame " << fcount << endl;
			}
		      gotcount++;
		    }
		}
	    }
	  else
	    { // fwanted > (fcount + 1) Sorry Charlie.
	      size_t rret =  fread( yuvbuf, yuvsize, 1, yuvinFP );
	      if ( rret != 1 ) {
		cerr << argv[0] << " stops. yuv stream ran only "<< fcount << " frames when frame "
		     << fwanted << " was wanted. ???" << endl;
		return 1;  //out of reading wanted frame number loop
	      }
	      fcount++;
	      if(vb) cerr << cmd << "Skip frame " << fcount << endl;
	      //We ignore the unwanted boring picture, overpaint its space with next.
	    }
	}  //Wanted list loop ends.
    }  
  if(vb) {
    cerr << cmd << " Done. "
	 << fcount << " frames read. "
	 << gotcount << " frames selected. Bye." << endl;
  }
  fclose(fnsFP);
  fclose(yuvinFP);
  return 0;
}  

static int get_our_options( int *argc, char **argv[])
{
  extern char *optarg; //globals from getopt and getopt_long
  extern int optind, opterr, optopt;

  int c;
  int digit_optind = 0;
  int num_args_gotten = 0; //for updating *argc and *argv
  //so original argument getting works

  //Logic here adapted from man 3 getopt
  while (1) {
    int this_option_optind = optind ? optind : 1;
    int option_index = 0;
    static struct option long_options[] = {
      {"verbose", optional_argument, &vb, 1},                           //0
      {"yuv-out-fd", required_argument, &do_yuv_stream_out, 1},   //1
      {"bmp-out-fd", required_argument, &do_bmp_stream_out, 1},   //2
      {"bmp-out-dirpath", required_argument, &do_bmp_dir_out, 1}, //3
      {"bmp-conversion", required_argument, 0, 0 },               //4
      {"bmp-prefix", required_argument, 0, 0},                    //5
      {"seline-fmt", required_argument, 0, 0},                    //6
      {"offset", required_argument, 0, 0},                        //7
      {"compress", optional_argument, &do_cx, 1},                 //8
      //option must use = sign: --compress="cmd [args]"
      {"nkids", required_argument, 0, 0},                         //9
      {0,         0,                 0,  0 }
    };
    c = getopt_long( *argc, *argv, "",
		    long_options, &option_index);
    if (c == -1)
      break;
    int ret;
    switch (c) {
    case 0:
      switch( option_index ) {
      case 1:
	if(vb) cerr << "--yuv-out-fd arg=" << optarg << endl;
	ret = sscanf(optarg, "%d", &yuvoutfd);
	if(vb) cerr << "--yuv-out-fd int=" << yuvoutfd << endl;
	if (ret != 1 )
	  {
	    error(1, 0, "--yuv-fd %s BAD, arg should be a fd int",
		  optarg);
	  }
	break;
      case 2:
	ret = sscanf(optarg, "%d", &bmpoutfd);
	if (ret != 1)
	  {
	    error(1, 0, "--bmp-fd %s BAD, arg should be a fd int",
		  optarg);
	  }
	break;
      case 3:
	bmpdirpaths = string(optarg);
	break;
      case 4:
	bmpconversions = optarg;
	break;
      case 5:
	bmpnameprefixs = string(optarg);
	break;
      case 6:
	selinefmt = optarg;
	break;
      case 7:
	ret = sscanf(optarg, "%d", &offset);
	if(ret == 0)
	  {
	    error(1, 0, "badly formatted --offset %s.", optarg);
	  }
	if( offset < 0 )
	  {
	    error(1, 0, "Negative --offset %d not supported (yet).", offset);
	  }
	break;
      case 8:
	if( optarg ) {
	  (void) mk_cx_argv( optarg );
	}
	break;
      case 9:
	cx_maxkids = atoi(optarg);
	if( cx_maxkids <= 0 ) {
	  error(1, 0, "Badly formatted or negative max number %d of compressing children.\n", cx_maxkids);
	}
	break;
      }
    }
  }

  char *cmd = (*argv)[0];
  *argc = *argc - (optind - 1);
  *argv = *argv + (optind - 1);
  (*argv)[0] = cmd;
  return 0;
}


