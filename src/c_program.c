#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Error handler. Should probably get this from a library.
void check(int ret, const char *message) {
  if (ret != -1) {
    return;
  }
  perror(message);
  exit(errno);
}

int main(__attribute__((unused)) int argc, char *argv[]) {
  char buffer[4096];
  int fd = atoi(argv[1]);
  int bytes_read = read(fd, buffer, sizeof(buffer));
  check(bytes_read, "read");
  printf("Message from Python: %.*s\n", bytes_read, buffer);
  close(fd);
}
