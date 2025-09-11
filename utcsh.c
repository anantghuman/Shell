/*
  utcsh - The UTCS Shell

  <Put your name and CS login ID here>
*/

/* Read the additional functions from util.h. They may be beneficial to you
in the future */
#include <ctype.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "util.h"

/* Global variables */
/* The array for holding shell paths. Can be edited by the functions in
 * util.c*/
char shell_paths[MAX_ENTRIES_IN_SHELLPATH][MAX_CHARS_PER_CMDLINE];
static char prompt[] = "utcsh> "; /* Command line prompt */
static char *default_shell_path[2] = { "/bin", NULL };
/* End Global Variables */

/* Convenience struct for describing a command. Modify this struct as you see
 * fit--add extra members to help you write your code. */
struct Command
{
  char **args; /* Argument array for the command */
};

typedef struct CommandNode
{
  struct Command cmd;
  struct CommandNode *next;
} cmd_node;

/* Here are the functions we recommend you implement */

char **tokenize_command_line (char *cmdline);
struct Command parse_command (char **tokens);
int eval (struct Command *cmd);
int try_exec_builtin (struct Command *cmd);
int exec_external_cmd (struct Command *cmd);
void printerr ();

/* Main REPL: read, evaluate, and print. This function should remain relatively
   short: if it grows beyond 60 lines, you're doing too much in main() and
   should try to move some of that work into other functions. */
int main (int argc, char **argv)
{
  set_shell_path (default_shell_path);
  char *lineptr = NULL;
  size_t n = 0;

  FILE *f;
  if (argc == 2)
    {
      f = fopen (argv[1], "r");
    }
  else if (argc == 1)
    {
      f = stdin;
    }
  else
    {
      printerr ();
      exit (1);
    }

  int c = fgetc (f);
  if (c == EOF)
    {
      printerr ();
      exit (1);
    }
  ungetc (c, f);

  while (true)
    {
      if (f == stdin)
        {
          printf ("%s", prompt);
        }

      if (getline (&lineptr, &n, f) == -1)
        {
          if (strlen (lineptr) == 0)
            {
              printerr ();
              free (lineptr);
              exit (1);
            }
          free (lineptr);
          exit (0);
        }

      if (isspace (lineptr[strlen (lineptr) - 1]))
        {
          lineptr[strlen (lineptr) - 1] = '\0';
        }

      char **tokens = tokenize_command_line (lineptr);

      struct Command cmd = parse_command (tokens);
      if (cmd.args == NULL)
        {
          free (tokens);
          continue;
        }

      int err = eval (&cmd);
      free (tokens);
      if (err == 0)
        {
          free (lineptr);
          exit (err);
        }
    }
  free (lineptr);
  return 0;
}

cmd_node *create_cmd_chain (struct Command cmd)
{
  int index = 0;
  int curr_arg = 0;
  cmd_node *head = malloc (sizeof (cmd_node));
  head->cmd.args = NULL;
  while (cmd.args[index] != NULL)
    {
      if (strcmp (cmd.args[index], "&") == 0)
        {
          break;
        }
      index++;
    }
  head->cmd.args = malloc ((index + 1) * sizeof (char *));
  head->cmd.args[index] = NULL;
  for (int k = 0; k < index; k++)
    {
      head->cmd.args[k] = cmd.args[k];
    }

  head->next = NULL;

  int i = 0;
  cmd_node *curr_node = head;
  while (cmd.args[i] != NULL)
    {
      if (strcmp (cmd.args[i], "&") == 0)
        {
          i++;

          if (cmd.args[i] == NULL || strcmp (cmd.args[i], "&") == 0)
            {
              continue;
            }

          curr_node->next = malloc (sizeof (cmd_node));
          curr_node = curr_node->next;

          int num_chars_til_amp = i;
          while (cmd.args[num_chars_til_amp] != NULL)
            {
              if (strcmp (cmd.args[num_chars_til_amp], "&") == 0)
                {
                  break;
                }
              num_chars_til_amp++;
            }
          curr_node->cmd.args
              = malloc ((num_chars_til_amp - i + 1) * sizeof (char *));
          curr_node->cmd.args[num_chars_til_amp - i] = NULL;
          curr_node->next = NULL;
          curr_arg = 0;
          continue;
        }
      curr_node->cmd.args[curr_arg] = cmd.args[i];
      curr_arg++;
      i++;
    }
  return head;
}

/* NOTE: In the skeleton code, all function bodies below this line are dummy
implementations made to avoid warnings. You should delete them and replace them
with your own implementation. */

/** Turn a command line into tokens with strtok
 *
 * This function turns a command line into an array of arguments, making it
 * much easier to process. First, you should figure out how many arguments you
 * have, then allocate a char** of sufficient size and fill it using strtok()
 */
char **tokenize_command_line (char *lineptr)
{
  int num_args = 0;
  for (size_t i = 0; i < strlen (lineptr); i++)
    {
      while (isspace (lineptr[i]))
        {
          lineptr[i] = ' ';
          i++;
        }
      num_args++;
    }

  char **tokens = malloc ((num_args + 2) * sizeof (char *));
  char *token = strtok (lineptr, " ");
  int index = 0;
  while (index <= num_args)
    {
      tokens[index] = token;
      token = strtok (NULL, " ");
      index++;
    }
  tokens[index] = NULL;
  return tokens;
}

/** Turn tokens into a command.
 *
 * The `struct Command` represents a command to execute. This is the preferred
 * format for storing information about a command, though you are free to
 * change it. This function takes a sequence of tokens and turns them into a
 * struct Command.
 */
struct Command parse_command (char **tokens)
{
  struct Command dummy = { .args = tokens };
  char *cmd_name = tokens[0];
  if (cmd_name == NULL)
    {
      dummy.args = NULL;
      return dummy;
    }
  if (strcmp (cmd_name, "exit") == 0)
    {
      dummy.args = tokens;
    }
  else if (strcmp (cmd_name, "cd") == 0)
    {
      if (tokens[1] == NULL || (tokens[1] != NULL && tokens[2] != NULL))
        {
          printerr ();
          dummy.args = NULL;
          return dummy;
        }
      dummy.args = tokens;
    }
  else if (strcmp (cmd_name, "path") == 0)
    {
      dummy.args = tokens;
    }
  return dummy;
}

void free_cmd_chain (cmd_node *head)
{
  cmd_node *h = head;
  while (h != NULL)
    {
      free (h->cmd.args);
      cmd_node *next = h->next;
      free (h);
      h = next;
    }
}

/** Evaluate a single command
 *
 * Both built-ins and external commands can be passed to this function--it
 * should work out what the correct type is and take the appropriate action.
 */
int eval (struct Command *cmd)
{
  // (void) cmd;
  cmd_node *head = create_cmd_chain (*cmd);
  cmd_node *cmd_chain = head;

  int i = 0;
  bool external = false;
  while (cmd_chain != NULL)
    {
      i++;
      int builtin_err = try_exec_builtin (&cmd_chain->cmd);
      if (builtin_err == 0)
        {
          external = true;
          int extern_err = exec_external_cmd (&cmd_chain->cmd);
          if (extern_err == 0)
            {
              free_cmd_chain (head);
              return extern_err;
            }
        }
      else if (builtin_err == -1)
        {
          free_cmd_chain (head);
          return 0;
        }
      cmd_chain = cmd_chain->next;
    }

  if (!external)
    {
      free_cmd_chain (head);
      return 1;
    }

  for (int j = 0; j < i; j++)
    {
      wait (NULL);
    }

  free_cmd_chain (head);
  return 1;
}

/** Execute built-in commands
 *
 * If the command is a built-in command, execute it and return 1 if appropriate
 * If the command is not a built-in command, do nothing and return 0
 */
int try_exec_builtin (struct Command *cmd)
{
  char *cmd_name = cmd->args[0];
  if (cmd->args[0] == NULL)
    {
      return 1;
    }

  if (strcmp (cmd_name, "exit") == 0)
    {
      if (cmd->args[1] != NULL)
        {
          printerr ();
          return 1;
        }
      return -1;
    }
  else if (strcmp (cmd_name, "cd") == 0)
    {
      int err = chdir (cmd->args[1]);
      char *path = getcwd (NULL, 0);
      if (err == -1)
        {
          printerr ();
        }
      free (path);
      return 1;
    }
  else if (strcmp (cmd_name, "path") == 0)
    {
      int err = set_shell_path (&cmd->args[1]);
      if (err == 0)
        {
          printerr ();
        }
      return 1;
    }
  return 0;
}

/** Execute an external command
 *
 * Execute an external command by fork-and-exec. Should also take care of
 * output redirection, if any is requested
 */
int exec_external_cmd (struct Command *cmd)
{
  pid_t pid = fork ();
  char *cmd_name = cmd->args[0];

  if (pid == 0)
    {
      if (is_absolute_path (cmd_name))
        {
          execv (cmd_name, cmd->args);
        }
      for (int i = 0; i < MAX_ENTRIES_IN_SHELLPATH; i++)
        {
          char *full_path
              = exe_exists_in_dir (shell_paths[i], cmd_name, false);
          if (full_path != NULL)
            {
              int j = 0;
              while (cmd->args[j] != NULL)
                {
                  if (strcmp (cmd->args[j], ">") == 0)
                    {
                      if (cmd->args[j + 1] != NULL && cmd->args[j + 2] == NULL)
                        {
                          int fd = open (cmd->args[j + 1],
                                         O_CREAT | O_WRONLY | O_TRUNC, 0644);

                          if (fd < 0)
                            {
                              printerr ();
                              exit (1);
                            }

                          dup2 (fd, STDOUT_FILENO);
                          dup2 (fd, STDERR_FILENO);

                          close (fd);
                          cmd->args
                              = realloc (cmd->args, (j + 1) * sizeof (char *));
                          cmd->args[j] = NULL;
                          break;
                        }
                      else
                        {
                          printerr ();
                          exit (1);
                        }
                    }
                  j++;
                }
              execv (full_path, cmd->args);
            }
          free (full_path);
        }

      // check if cmd is all ampersands
      bool all_ampersands = true;
      for (int k = 0; cmd_name[k] != '\0'; k++)
        {
          if (cmd_name[k] != '&')
            {
              all_ampersands = false;
              break;
            }
        }
      if (!all_ampersands)
        {
          printerr ();
        }
      exit (1);
    }
  return 1;
}

void printerr ()
{
  // pulled code from part 2.4 of shell project document
  char emsg[30] = "An error has occurred\n";
  write (STDERR_FILENO, emsg, strlen (emsg));
}
