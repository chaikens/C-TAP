#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <error.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

//link with -lpthread

int main(int argc, char *argv[])
{
  char name[NAME_MAX-3];
  name[0] = '/';
  strcpy(name+1, argv[1]);
  
  int ret = sem_unlink(name);
  if ( ret ) {
    error(ret, errno, "My semclose failed.");
    }
  return 0;
}
  
