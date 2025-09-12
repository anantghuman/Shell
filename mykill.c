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
  pid_t pid1 = atoi (argv[1]);
  int k = kill (pid1, SIGUSR1);
  printf ("%d", k);
  return 0;
}
