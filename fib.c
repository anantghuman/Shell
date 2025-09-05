#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

const int MAX = 13;

static void doFib (int n, int doPrint);

/*
 * unix_error - unix-style error routine.
 */
inline static void unix_error (char *msg)
{
  fprintf (stdout, "%s: %s\n", msg, strerror (errno));
  exit (1);
}

int main (int argc, char **argv)
{
  int arg;
  int print = 1;

  if (argc != 2)
    {
      fprintf (stderr, "Usage: fib <num>\n");
      exit (-1);
    }

  arg = atoi (argv[1]);
  if (arg < 0 || arg > MAX)
    {
      fprintf (stderr, "number must be between 0 and %d\n", MAX);
      exit (-1);
    }

  doFib (arg, print);

  return 0;
}

static void doFibHelper(int n, int doPrint) {
  if (n == 0 || n == 1) {
    printf("%d", 1);
    exit(1);
  }

  int left = 0, right = 0;
  char* arr[3];
  arr[0] = "fib";
  arr[2] = NULL;

  pid_t pid1 = fork();

  if (pid1 == 0) {
    sprintf(arr[1], "%d", n - 1);
    execvp(arr[0], arr);
    wait(&left);
  }

  pid_t pid2 = fork();
  if (pid2 == 0) {
    sprintf(arr[1], "%d", n - 2);
    execvp(arr[0], arr);
    // wait(&right);
  } else {
    wait(&right);
  }

  printf("%d %d", left + right, doPrint);
  exit(left + right);
}

/*
 * Recursively compute the specified number. If print is
 * true, print it. Otherwise, provide it to my parent process.
 *
 * NOTE: The solution must be recursive and it must fork
 * a new child for each call. Each process should call
 * doFib() exactly once.
 */

 /*
 n = 0: 1
 n = 1: 1
 fork()
 exec(n-1)
 exec(n-2)
 return wait(pid1 + pid2)
 */
static void doFib (int n, int doPrint) {
  if (n > 13) {
    fprintf(stderr, "n is too large\n");
    exit(-1);
  }
  doFibHelper(n, 0);
}

