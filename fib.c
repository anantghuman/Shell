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
  printf("n: %d\n", n);
  if (n == 0) {
    exit(0);
  }
  if (n == 1) {
    exit(1);
  }

  int left = 0, right = 0;
  char* arr[3];
  arr[0] = "fib";
  arr[2] = NULL;
  arr[1] = malloc(2 * sizeof(char));
  pid_t pid1 = fork();

  if (pid1 == 0) {
    // printf("in child process\n");
    sprintf(arr[1], "%d", n - 1);
    // printf("execing %s %s\n", arr[0], arr[1]);
    execvp(arr[0], arr);
  } else {
    // printf("in parent process\n");
    wait(&left);
    // printf("error");
  }

  pid_t pid2 = fork();
  if (pid2 == 0) {
    sprintf(arr[1], "%d", n - 2);
    execvp(arr[0], arr);
  } else {
    wait(&right);
  }
  printf("total: %d", left + right);
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
  doFibHelper(n, 1);
  int result;
  wait(&result);
  if (doPrint) {
    printf("fib(%d) = %d\n", n, result);
  } else {
    int result;
    wait(&result);
    exit(result);
  }
}