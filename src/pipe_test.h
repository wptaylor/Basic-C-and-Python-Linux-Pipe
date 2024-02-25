#include <sys/types.h>

static pid_t python_pid, c_pid;

void check(int, const char *);
void child_bad_exit_handler(int, pid_t);

void handle_signal(int);
void register_signal(int);

void parent_branch(void);
void python_child(int, int);
void c_child(int, int);
