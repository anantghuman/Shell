#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include "util.h"


/*
 * First, print out the process ID of this process.
 *
 * Then, set up the signal handler so that ^C causes
 * the program to print "Nice try.\n" and continue looping.
 *
 * Finally, loop forever, printing "Still here\n" once every
 * second.
 */
int main(int argc, char **argv)
{
  pid_t pid = getpid();
  printf("%d\n", pid);
  init_signals();
  struct timespec time = {1, 0};
  while (1) {
    ssize_t bytes;
    const int STDOUT = 1;
    bytes = write(STDOUT, "Nice try.\n", 10);
    if(bytes != 10) {
      exit(-999);
    }
    nanosleep(&time, NULL);
  }
}
/*

signal handler () {
  set sigusr1 to sigkill function
  set sigkill to printNiceTry function
  }
*/
void init_signals() {
  struct sigaction sa;
  sa.sa_handler = signal_handler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGINT, &printNiceTry, NULL);
}

void printNiceTry() {
  const int STDOUT = 1;
  ssize_t bytes;
  bytes = write(STDOUT, "Nice try.\n", 10);
  if(bytes != 10) {
    exit(-999);
  }
}




