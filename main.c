#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

#include "server.h"
#include "signal_handlers.h"


int main(int argc, char *argv[])
{
  struct sigaction sa_chld, sa_int;
  struct addrinfo hints, *res, *p;
  struct sockaddr *client_address;
  socklen_t *client_address_len;
  int status, client_socket, yes = 1;
  char *message;

  sa_int.sa_handler = interrupt_signal_handler;
  sigemptyset(&sa_int.sa_mask);
  if (sigaction(SIGINT, &sa_int, NULL) == -1) {
    perror("sigaction SIGNINT");
    exit(1);
  }

  sa_chld.sa_handler = child_signal_handler;
  sigemptyset(&sa_chld.sa_mask);
  sa_chld.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa_chld, NULL) == -1) {
    perror("sigaction SIGCHLD");
    exit(1);
  }

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if ((status = getaddrinfo(NULL, PORT, &hints, &res)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    return 2;
  }

  for (p = res; p != NULL; p = p->ai_next) {
    server_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (server_socket == -1) {
      fprintf(stderr, "Not able to create server socket.\n");
    }

    if (setsockopt(server_socket, SOL_SOCKET,
                   SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      fprintf(stderr, "Not able to set socket options.\n");
    }

    if (bind(server_socket, p->ai_addr, p->ai_addrlen) == -1) {
      close(server_socket);
      fprintf(stderr, "Not able to bind server socket to host address.\n");
    }

    if (listen(server_socket, BACKLOG) == -1) {
      fprintf(stderr, "Not able to listen on the socket.\n");
      close(server_socket);
    }

    break;
  }

  freeaddrinfo(res);

  client_address = NULL;
  client_address_len = 0;

  while (1) {
    client_socket = accept(server_socket, client_address, client_address_len);
    if (client_socket < 0) {
      fprintf(stderr, "Not able to accept connection.\n");
    } else {
      fprintf(stdout, "Got a new connection.\n");
    }
    if (fork()) { /* parent process */
      close(client_socket);
      free(client_address);
      free(client_address_len);
    } else { /* child process */
      close(server_socket);
      message = "Howdy?\n"; /* What if this message is dynamic? */
      send(client_socket, message, strlen(message), 0);
      close(client_socket);
      exit(0);
    }
  }

  close(server_socket);
  return 0;
}
