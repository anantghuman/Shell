/*
  utcsh - The UTCS Shell

  <Put your name and CS login ID here>
*/

/* Read the additional functions from util.h. They may be beneficial to you
in the future */
#include "util.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
// #include <sys/types.h>
#include <unistd.h>

/* Global variables */
/* The array for holding shell paths. Can be edited by the functions in util.c*/
char shell_paths[MAX_ENTRIES_IN_SHELLPATH][MAX_CHARS_PER_CMDLINE];
static char prompt[] = "utcsh> "; /* Command line prompt */
static char *default_shell_path[2] = {"/bin", NULL};
/* End Global Variables */

/* Convenience struct for describing a command. Modify this struct as you see
 * fit--add extra members to help you write your code. */
struct Command
{
  char **args;      /* Argument array for the command */
  char *outputFile; /* Redirect target for file (NULL means no redirect) */
};

/* Here are the functions we recommend you implement */

char **tokenize_command_line (char *cmdline);
struct Command parse_command (char **tokens);
void eval (struct Command *cmd);
int try_exec_builtin (struct Command *cmd);
void exec_external_cmd (struct Command *cmd);
void printerr(char *msg);

/* Main REPL: read, evaluate, and print. This function should remain relatively
   short: if it grows beyond 60 lines, you're doing too much in main() and
   should try to move some of that work into other functions. */
int main (int argc, char **argv)
{
  set_shell_path (default_shell_path);
  char* lineptr = NULL;
  size_t n = 0;
  while (1)
    {
      printf ("%s", prompt);
      if (getline(&lineptr, &n, stdin) == -1) {
        exit(0);
      }

      int num_args = 0;
      for (int i = 0; i < strlen(lineptr); i++) {
        if (lineptr[i] == ' ') {
          num_args++;
        }
      }

      char** tokens = malloc((num_args + 2) * sizeof(char*));
      char* token = strtok(lineptr, " ");
      int index = 0;
      while (index <= num_args) {
        tokens[index] = token;
        token = strtok(NULL, " ");
        index++;
      }
      tokens[index] = NULL;
      // free(lineptr);

      struct Command cmd = parse_command(tokens);
      eval(&cmd);

      printf ("If you see these lines, you are probably running the shell "
              "skeleton. Exiting to prevent terminal spam.\n");
      exit (1883);

      /* Read */

      /* Evaluate */

      /* Print (optional) */
    }
  return 0;
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
char **tokenize_command_line (char *cmdline)
{
  (void) cmdline;
  return NULL;
}

/** Turn tokens into a command.
 *
 * The `struct Command` represents a command to execute. This is the preferred
 * format for storing information about a command, though you are free to change
 * it. This function takes a sequence of tokens and turns them into a struct
 * Command.
 */
struct Command parse_command (char **tokens)
{
  struct Command dummy = {.args = tokens, .outputFile = NULL};
  char* cmd_name = tokens[0];

  if (strcmp(cmd_name, "exit") == 0) {
    dummy.args = NULL;
    dummy.args = tokens;
  } else if (strcmp(cmd_name, "cd") == 0) {
    if (tokens[1] && tokens[2] != NULL) {
      char emsg[30] = "An error has occurred\n";
      int nbytes_written = write(STDERR_FILENO, emsg, strlen(emsg));
      return dummy;
    }
    dummy.args = tokens;
  } else if (strcmp(cmd_name, "path") == 0) {
    dummy.args = tokens;
  }

  return dummy;
}
/** Evaluate a single command
 *
 * Both built-ins and external commands can be passed to this function--it
 * should work out what the correct type is and take the appropriate action.
 */
void eval (struct Command *cmd)
{
  // (void) cmd;
  int err = try_exec_builtin(cmd);
  if (err == 0) {
    exec_external_cmd(cmd);
  }
  return;
}

/** Execute built-in commands
 *
 * If the command is a built-in command, execute it and return 1 if appropriate
 * If the command is not a built-in command, do nothing and return 0
 */
int try_exec_builtin (struct Command *cmd)
{
  // (void) cmd;
  char *cmd_name = cmd->args[0];
  bool s = strcmp(cmd_name, "cd");
  if (s) {
      printerr("true");
  } else {
      printerr(cmd_name);
      printerr("cd");
      printerr("false");
    }
  // INFO: exited in main
  if (strcmp(cmd_name, "exit") == 0) {
    if (cmd->args[1] != NULL) {
      printerr("an error has occurred\n");
      return 1;
    }
    exit(0);
  } else if (strcmp(cmd_name, "cd")) {
    printerr("adaisjk");
    int err = chdir(cmd->args[1]);
    printerr(cmd->args[1]);
    if (err == -1) {
      printerr("an error has occurred\n");
    }
    return 1;
  } else if (strcmp(cmd_name, "path")) {
    int err = set_shell_path(&cmd->args[1]);
    if (err == 0) {
      printerr("an error has occurred\n");
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
void exec_external_cmd (struct Command *cmd)
{
  pid_t pid = fork();
  char* cmd_name = cmd->args[0];

  if (pid == 0) {
    for (int i = 0; i < MAX_ENTRIES_IN_SHELLPATH; i++) {
      char* full_path = malloc(strlen(shell_paths[i]) + strlen(cmd_name) + 2);
      strcpy(full_path, shell_paths[i]);
      strcat(full_path, "/");
      strcat(full_path, cmd_name);
      if (access(full_path, X_OK) == 0) {
        execv(full_path, &cmd->args[1]);
      }
    }
  } else {
    waitpid(pid, NULL, 0);
  }
}

void printerr(char *msg) {
  char emsg[30] = "An error has occurred\n";
  int nbytes_written = write(STDERR_FILENO, msg, strlen(msg));
}
