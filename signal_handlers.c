#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "server.h"

void interrupt_signal_handler(int signum)
{
  fprintf(stdout, "Caught signal %d\n", signum);
  fprintf(stdout, "Closing socket %d\n", server_socket);
  if (close(server_socket) < 0) {
    perror("Failed to close server_socke (ignoring)\n");
  }
  exit(0);
}


void child_signal_handler(int signum) {
  int saved_errno = errno;
  while (waitpid(-1, NULL, WNOHANG) > 0);
  errno = saved_errno;
}
