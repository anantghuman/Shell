#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>


int main(int argc, char **argv)
{
  pid_t pid1 = atoi(argv[1]);
  int k = kill(pid1, SIGUSR1);
  printf("%d", k);
  // exit(EXIT_SUCCESS);
  return 0;
}
