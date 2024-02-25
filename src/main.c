
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// Error handler.
void check(int ret, const char *message) {
  if (ret != -1) {
    return;
  }
  int error = errno;
  perror(message);
  exit(error);
}

// C-code prototype.
void parent_branch(int fds[2], pid_t child_pid) {
  char buffer[4096];
  int bytes_read = read(fds[0], buffer, sizeof(buffer));
  check(bytes_read, "read");
  printf("Message from Python: %.*s\n", bytes_read, buffer);

  int status;
  waitpid(child_pid, &status, 0);
  close(fds[0]);
  printf("Python terminated with status code %d.\n", status);
  exit(status);
}

// Python prototype.
void child_branch(int fds[2]) {
  // Map stdout to the pipe's input.
  dup2(fds[1], STDOUT_FILENO);
  close(fds[1]);

  // Call Python script with no arguments.
  char *exec_arg = {NULL};
  execlp("src/main.py", exec_arg); // Alternatively: system("src/main.py")
}

int main(void) {
  printf("Start of Program.\n");

  int fds[2];
  check(pipe(fds), "pipe");

  pid_t fork_pid = fork();
  check(fork_pid, "fork");
  if (fork_pid > 0) {
    parent_branch(fds, fork_pid);
  } else {
    child_branch(fds);
  }
}
