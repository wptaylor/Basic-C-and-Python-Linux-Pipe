
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static pid_t python_pid, c_pid;

// Error handler.
void check(int ret, const char *message) {
  if (ret != -1) {
    return;
  }
  int error = errno;
  perror(message);
  exit(error);
}

// Signal handler.
void handle_signal(__attribute__((unused)) int signum) {
  int wstatus;
  pid_t child_pid = waitpid(-1, &wstatus, WNOHANG);
  if (wstatus == 0) {
    return;
  }
  if (child_pid == python_pid) {
    printf("Python terminated with status %d.\n", wstatus);
  } else if (child_pid == c_pid) {
    printf("C terminated with status code %d.\n", wstatus);
  } else {
    printf("PID error. Program terminated with status code %d.\n", wstatus);
  }
}

// Setup signal handler.
void register_signal(int signum) {
  struct sigaction new_action = {0};
  sigemptyset(&new_action.sa_mask);
  new_action.sa_handler = handle_signal;
  check(sigaction(signum, &new_action, NULL), "sigaction");
}

// Manages children.
void parent_branch(void) {
  pause(); // Waits for first child.
  pause(); // Waits for second child.
  printf("End of program.\n");
  exit(errno);
}

// Python prototype.
void python_child(int fds[2]) {
  // Call Python script with a single argument for the write file descriptor.
  char fd_out[6];
  sprintf(fd_out, "%d", fds[1]); // Convert fds[1] to char array.
  char *exec_argv[] = {"python_program.py", fd_out, NULL};
  char *exec_envp[] = {NULL};
  check(execve("src/python_program.py", exec_argv, exec_envp), "execve python");
}

// C prototype.
void c_child(int fds[2]) {
  // Call C program with a single argument for the read file descriptor.
  char fd_in[6];
  sprintf(fd_in, "%d", fds[0]); // Convert fds[0] to char array.
  char *exec_argv[] = {"c_program", fd_in, NULL};
  char *exec_envp[] = {NULL};
  check(execve("build/c_program", exec_argv, exec_envp), "execve c");
}

int main(void) {
  printf("Start of Program.\n");

  register_signal(SIGCHLD);

  int fds[2];
  check(pipe(fds), "pipe");

  pid_t fork_pid = fork();
  check(fork_pid, "fork 1");
  if (fork_pid > 0) {
    python_pid = fork_pid;
    fork_pid = fork();
    check(fork_pid, "fork 2");
  } else {
    python_child(fds);
  }
  if (fork_pid > 0) {
    c_pid = fork_pid;
    parent_branch();
  } else {
    c_child(fds);
  }
}
