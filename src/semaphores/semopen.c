#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <error.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>

//link with -lpthread, 

int main(int argc, char *argv[])
{
  if (argc != 3) {
    error(1,0,"semopen <semaphore name> <init value>");
  }
  char name[NAME_MAX-3];
  name[0] = '/';
  strcpy(name+1, argv[1]);
  
  int ival = atoi(argv[2]);
  if ( ival < 0 ) {
    error(1,0,"Bad negative initial value for your semaphore.");
      }
  sem_t *psem = sem_open(name, O_CREAT, 0644, ival);
  if ( psem == SEM_FAILED ) {
    error(1, errno, "My seminit failed");
    }
  return 0;
}
  
