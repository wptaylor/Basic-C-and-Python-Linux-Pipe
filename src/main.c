#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "pipe_test.h"

int main(void) {
  printf("Start of Program.\n");

  register_signal(SIGCHLD);

  int fds_python_to_c[2];
  int fds_c_to_python[2];
  check(pipe(fds_python_to_c), "pipe p->c");
  check(pipe(fds_c_to_python), "pipe c->p");

  pid_t fork_pid = fork();
  check(fork_pid, "fork 1");
  if (fork_pid > 0) {
    python_pid = fork_pid;
    fork_pid = fork();
    check(fork_pid, "fork 2");
  } else {
    python_child(fds_c_to_python[0], fds_python_to_c[1]);
  }
  if (fork_pid > 0) {
    c_pid = fork_pid;
    parent_branch();
  } else {
    c_child(fds_python_to_c[0], fds_c_to_python[1]);
  }
}

// Manages children.
void parent_branch(void) {
  pause(); // Waits for first child.
  pause(); // Waits for second child.
  printf("End of program.\n");
  exit(errno);
}

// Python prototype.
void python_child(int read_fd, int write_fd) {
  char read_fd_arg[6];
  char write_fd_arg[6];
  sprintf(read_fd_arg, "%d", read_fd);
  sprintf(write_fd_arg, "%d", write_fd);
  char *exec_argv[] = {"python_program.py", read_fd_arg, write_fd_arg, NULL};
  char *exec_envp[] = {NULL};
  check(execve("src/python_program.py", exec_argv, exec_envp), "execve python");
}

// C prototype.
void c_child(int read_fd, int write_fd) {
  char read_fd_arg[6];
  char write_fd_arg[6];
  sprintf(read_fd_arg, "%d", read_fd);
  sprintf(write_fd_arg, "%d", write_fd);
  char *exec_argv[] = {"c_program", read_fd_arg, write_fd_arg, NULL};
  char *exec_envp[] = {NULL};
  check(execve("build/c_program", exec_argv, exec_envp), "execve c");
}

// Error handler.
void check(int ret, const char *message) {
  if (ret != -1) {
    return;
  }
  perror(message);
  exit(errno);
}

void child_bad_exit_handler(int wstatus, pid_t child_pid) {
  char *program_type;
  if (child_pid == python_pid) {
    program_type = "Python";
  } else if (child_pid == c_pid) {
    program_type = "C";
  } else {
    program_type = "Unknown program";
  }
  fprintf(stderr, "%s terminated with status code %d\n", program_type, wstatus);
  exit(errno);
}

// Signal handler. Only used for preventing zombie children.
void handle_signal(__attribute__((unused)) int signum) {
  int wstatus;
  pid_t child_pid = waitpid(-1, &wstatus, WNOHANG);
  if (wstatus == 0) {
    return;
  }
  child_bad_exit_handler(wstatus, child_pid);
}

// Setup signal handler.
void register_signal(int signum) {
  struct sigaction new_action = {0};
  sigemptyset(&new_action.sa_mask);
  new_action.sa_handler = handle_signal;
  check(sigaction(signum, &new_action, NULL), "sigaction");
}
