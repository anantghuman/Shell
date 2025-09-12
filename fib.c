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

static void doFibHelper (int n, int doPrint)
{
  if (n == 0)
    {
      if (doPrint == 1)
        {
          printf ("0\n");
        }
      exit (0);
    }
  if (n == 1)
    {
      if (doPrint == 1)
        {
          printf ("1\n");
        }
      exit (1);
    }

  int left = 0, right = 0;
  pid_t pid1 = fork ();

  if (pid1 == 0)
    {
      doFibHelper (n - 1, 0);
    }
  else
    {
      waitpid (pid1, &left, 0);
      left = WEXITSTATUS (left);
    }

  pid_t pid2 = fork ();
  if (pid2 == 0)
    {
      doFibHelper (n - 2, 0);
    }
  else
    {
      waitpid (pid2, &right, 0);
      right = WEXITSTATUS (right);
    }

  if (doPrint == 1)
    {
      printf ("%d\n", left + right);
    }
  else
    {
      exit (left + right);
    }
}

/*
 * Recursively compute the specified number. If print is
 * true, print it. Otherwise, provide it to my parent process.
 *
 * NOTE: The solution must be recursive and it must fork
 * a new child for each call. Each process should call
 * doFib() exactly once.
 */

static void doFib (int n, int doPrint)
{
  doFibHelper (n, doPrint);
}
