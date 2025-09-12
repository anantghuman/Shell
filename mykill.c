#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main (int argc, char **argv)
{
  // alex driving here
  int pid1 = atoi (argv[1]);
  int k = kill (pid1, SIGUSR1);
  return 0;
}
