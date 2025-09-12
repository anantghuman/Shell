#include "util.h"
#include "sig_util.h"
#include <assert.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// anant driving here
void printNiceTry (int signo)
{
  // pulled code from part 1.2 of shell project instructions
  const int STDOUT = 1;
  ssize_t bytes;
  bytes = write (STDOUT, "Nice try.\n", 10);
  if (bytes != 10)
    {
      exit (-999);
    }
}

// alex driving here
void printExiting (int signo)
{
  // pulled code from part 1.2 of shell project instructions
  const int STDOUT = 1;
  write (STDOUT, "exiting\n", 8);
  exit (1);
}

/*
signal handler () {
  set sigusr1 to sigkill function
  set sigkill to printNiceTry function
  }
*/
void init_sig ()
{
  // anant and alex driving here
  signal_action(SIGINT, &printNiceTry);
  signal_action(SIGUSR1, &printExiting);
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
int main (int argc, char **argv)
{
  // anant driving here
  int pid = getpid ();
  printf ("%d\n", pid);
  init_sig ();
  struct timespec time = { 1, 0 };
  while (1)
    {
      ssize_t bytes;
      const int STDOUT = 1;
      bytes = write (STDOUT, "Still here\n", 11);
      if (bytes != 11)
        {
          exit (-999);
        }
      nanosleep (&time, NULL);
    }
}
