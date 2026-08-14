#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <error.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>

//link with -lpthread, -lbsd

int main(int argc, char *argv[])
{
  if (argc != 2) {
    error(1,0,"semup <name>");
  }
  char name[NAME_MAX-3];
  name[0] = '/';
  strcpy(name, argv[1]);
  sem_t *ps = sem_open(name, 0);
  if( ps == SEM_FAILED) {
    error(1, errno, "semup failed.");
  }
  sem_post(ps);
  return 0;
}
  
