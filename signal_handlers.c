#include <errno.h>
#include <signal.h>
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


void register_signal_handlers()
{
  struct sigaction sa_int, sa_chld;

  sa_int.sa_handler = interrupt_signal_handler;
  sigemptyset(&sa_int.sa_mask);
  if (sigaction(SIGINT, &sa_int, NULL) == -1) {
    perror("sigaction SIGINT");
    exit(1);
  }

  sa_chld.sa_handler = child_signal_handler;
  sigemptyset(&sa_chld.sa_mask);
  sa_chld.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa_chld, NULL) == -1) {
    perror("sigaction SIGCHLD");
    exit(1);
  }
}
