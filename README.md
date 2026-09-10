# UTCSH — Unix Shell in C

A lightweight Unix shell implemented from scratch in C, built to explore the operating-system primitives behind command execution: process creation, program loading, path resolution, synchronization, file descriptors, and I/O redirection.

## Features

* **Interactive shell** with a `utcsh>` prompt
* **Script execution** from a file
* **External command execution** using `fork()` and `execv()`
* **Configurable executable search path**
* Built-in commands:

  * `cd`
  * `path`
  * `exit`
* **Concurrent process execution** using `&`
* **Output redirection** using `>`
* Redirects both `stdout` and `stderr`
* Child-process synchronization using `wait()`
* Executable discovery and permission checking
* Automated regression test suite
* Debug and AddressSanitizer/UndefinedBehaviorSanitizer build configurations

## Architecture

UTCSH follows the basic lifecycle of a Unix command interpreter:

```text
Input
  │
  ▼
Tokenize command line
  │
  ▼
Parse command
  │
  ├── Built-in ────────► Execute inside shell
  │
  └── External command
          │
          ├── Resolve executable
          ├── fork()
          │     │
          │     └── Child
          │           ├── Configure redirection
          │           └── execv()
          │
          └── Parent
                └── wait()
```

For commands separated by `&`, the shell constructs each command first, launches the processes without immediately waiting, and then collects the child processes afterward. This allows the programs to execute concurrently.

## Building

### Requirements

* GCC
* GNU Make
* POSIX-compatible environment

Clone the repository:

```bash
git clone https://github.com/anantghuman/Shell.git
cd Shell
```

Build the shell:

```bash
make
```

This produces the `utcsh` executable.

### Debug Build

```bash
make debug
```

Compiles without optimization and includes additional debugging information.

### Sanitizer Build

```bash
make asan
```

Compiles with AddressSanitizer and UndefinedBehaviorSanitizer enabled.

### Clean

```bash
make clean
```

## Usage

### Interactive Mode

Start the shell with:

```bash
./utcsh
```

Example:

```text
utcsh> path /bin /usr/bin
utcsh> ls
utcsh> pwd
utcsh> exit
```

The shell initially searches `/bin` for executables. The `path` built-in replaces the current shell search path.

### Script Mode

UTCSH can also execute commands from a file:

```bash
./utcsh script.utcsh
```

For example:

```text
path /bin /usr/bin
echo Hello from UTCSH
pwd
ls
```

The repository includes example scripts under [`examples/`](examples/).

## Built-in Commands

### `cd`

Changes the shell's current working directory.

```text
cd <directory>
```

Example:

```text
cd /tmp
```

`cd` accepts exactly one directory argument.

### `path`

Replaces the list of directories UTCSH searches when resolving external programs.

```text
path <directory> [directory ...]
```

Example:

```text
path /bin /usr/bin
```

Clearing the path prevents commands without an explicit executable path from being resolved:

```text
path
```

### `exit`

Terminates the shell.

```text
exit
```

No arguments are accepted.

## Concurrent Commands

Commands separated with `&` are launched concurrently.

```text
sleep 2 & echo hello
```

Rather than waiting for the first process before creating the second, UTCSH launches the commands first and waits for their completion afterward.

This functionality is implemented using a linked list of parsed commands combined with `fork()` and `wait()`.

## Output Redirection

External command output can be redirected with:

```text
command > file
```

Example:

```text
path /bin
ls > files.txt
```

UTCSH opens the destination file and uses `dup2()` to redirect the child process's file descriptors before `execv()` replaces the child process image.

Both:

```text
stdout
stderr
```

are redirected to the destination file.

The destination is opened with truncation semantics, so an existing file is overwritten.

## Process Model

External programs are executed using the standard Unix process model:

```c
fork()
  ↓
child process
  ↓
resolve executable
  ↓
configure file descriptors
  ↓
execv()
```

The parent process later uses `wait()` to collect terminated children.

This keeps shell state such as the working directory and executable search path in the parent while allowing external programs to execute in isolated child processes.

## Executable Resolution

UTCSH supports executable lookup through its internal shell path.

Given:

```text
path /bin /usr/bin
```

and:

```text
ls
```

the shell searches each configured directory for an executable named `ls`.

Executable candidates are validated using filesystem traversal and `access(..., X_OK)` before execution.

Absolute executable paths can also be invoked directly:

```text
/bin/ls
```

## Testing

The repository contains an automated Python regression test suite covering behavior such as:

* built-in commands
* executable path resolution
* malformed commands
* script execution
* concurrent execution
* output redirection
* invalid redirection syntax
* empty input
* long inputs
* error behavior

Run the complete test suite with:

```bash
make check
```

Run an individual test with:

```bash
make testcase id=<test-id>
```

Inspect a test with:

```bash
make describe id=<test-id>
```

The testing framework and individual specifications are located under:

```text
tests/
├── run-tests.py
├── test-specs/
└── test-utils/
```

## Project Structure

```text
.
├── utcsh.c              # Main shell implementation
├── util.c               # Executable/path utilities
├── util.h
├── Makefile
│
├── examples/            # Example shell scripts
│
├── tests/               # Automated regression suite
│   ├── run-tests.py
│   ├── test-specs/
│   └── test-utils/
│
├── fib.c                # Process/fork exercise
├── handle.c             # Signal-handling utility
├── mykill.c             # Signal utility
├── sig_util.c
├── sig_util.h
│
├── shell_design.txt     # Design documentation
└── programming_log.shell
```

## Concepts Demonstrated

The project provides a low-level implementation of several core operating-system concepts:

**Process management**

* `fork()`
* `execv()`
* `wait()`
* exit-status propagation

**File descriptors**

* `open()`
* `close()`
* `dup2()`
* `stdout` / `stderr` redirection

**Filesystem interaction**

* directory traversal
* executable discovery
* permission checks
* working-directory management

**Shell design**

* tokenization
* command parsing
* built-in dispatch
* executable resolution
* concurrent command execution

## Limitations

UTCSH is intentionally a small educational shell rather than a full Bash-compatible interpreter. It does not aim to implement features such as:

* pipelines (`|`)
* shell variables or environment expansion
* command substitution
* globbing
* quoting/escaping semantics
* job control
* command history

The focus is on the operating-system mechanisms underneath basic shell execution.

## Authors

**Anant Ghuman**
**Alexander Wang**

Developed for UT Austin CS 439 — Principles of Computer Systems.
