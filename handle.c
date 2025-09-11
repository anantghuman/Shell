#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include "util.h"

void printNiceTry(int signo) {
  // pulled code from part 1.2 of shell project instructions
  const int STDOUT = 1;
  ssize_t bytes;
  bytes = write(STDOUT, "Nice try.\n", 10);
  if(bytes != 10) {
    exit(-999);
  }
}

void printExiting(int signo) {
  // pulled code from part 1.2 of shell project instructions
  const int STDOUT = 1;
  ssize_t bytes;
  bytes = write(STDOUT, "exiting.\n", 9);
  exit(1);
}

/*
signal handler () {
  set sigusr1 to sigkill function
  set sigkill to printNiceTry function
  }
*/
void init_sig() {
  struct sigaction sa;
  sa.sa_handler = &printNiceTry;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  if (sigaction(SIGINT, &sa, NULL) == -1) {
    printf("sigaction error");
    return;
  }

  struct sigaction sa2;
  sa2.sa_handler = &printExiting;
  sigemptyset(&sa2.sa_mask);
  sa2.sa_flags = 0;
  if (sigaction(SIGUSR1, &sa2, NULL) == -1) {
    printf("sigaction error");
    return;
  }
}

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
  init_sig();
  struct timespec time = {1, 0};
  while (1) {
    ssize_t bytes;
    const int STDOUT = 1;
    bytes = write(STDOUT, "Still here.\n", 12);
    if(bytes != 12) {
      exit(-999);
    }
    nanosleep(&time, NULL);
  }
}
