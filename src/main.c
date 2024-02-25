
#include <errno.h>
#include <signal.h>
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

void handle_signal(__attribute__((unused)) int signum) {
  int wstatus;
  waitpid(-1, &wstatus, WNOHANG);
  printf("Python terminated with status code %d.\n", wstatus);
}

void register_signal(int signum) {
  struct sigaction new_action = {0};
  sigemptyset(&new_action.sa_mask);
  new_action.sa_handler = handle_signal;
  check(sigaction(signum, &new_action, NULL), "sigaction");
}

// C-code prototype.
void parent_branch(int fds[2]) {
  char buffer[4096];
  int bytes_read = read(fds[0], buffer, sizeof(buffer));
  check(bytes_read, "read");
  printf("Message from Python: %.*s\n", bytes_read, buffer);

  pause();
  close(fds[0]);
  printf("Exiting parent.\n");
  exit(errno);
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
  register_signal(SIGCHLD);
  printf("Start of Program.\n");

  int fds[2];
  check(pipe(fds), "pipe");

  pid_t fork_pid = fork();
  check(fork_pid, "fork");
  if (fork_pid > 0) {
    parent_branch(fds);
  } else {
    child_branch(fds);
  }
}
