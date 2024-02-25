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
  int read_fd = atoi(argv[1]);
  int write_fd = atoi(argv[2]);

  char buffer[4096];
  int bytes_read = read(read_fd, buffer, sizeof(buffer));
  check(bytes_read, "read");
  printf("Message from Python: %.*s\n", bytes_read, buffer);
  close(read_fd);

  int bytes_written = write(write_fd, "I am a letter.", 15);
  check(bytes_written, "write");
  close(write_fd);
}
