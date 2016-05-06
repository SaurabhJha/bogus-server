#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "server.h"

int create_tcp_listener()
{
  struct addrinfo hints, *res, *p;
  int status, tcp_socket, yes = 1;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if ((status = getaddrinfo(NULL, PORT, &hints, &res)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    return 2;
  }

  tcp_socket = -1;
  for (p = res; p != NULL; p = p->ai_next) {
    tcp_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (tcp_socket == -1) {
      fprintf(stderr, "Not able to create server socket.\n");
    }

    if (setsockopt(tcp_socket, SOL_SOCKET,
                   SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
      fprintf(stderr, "Not able to set socket options.\n");
    }

    if (bind(tcp_socket, p->ai_addr, p->ai_addrlen) == -1) {
      fprintf(stderr, "Not able to bind server socket to address");
      close(tcp_socket);
    }

    if (listen(tcp_socket, BACKLOG) == -1) {
      fprintf(stderr, "Not able to listen on the socket.\n");
      close(tcp_socket);
    }

    break;
  }

  freeaddrinfo(res);

  return tcp_socket;
}
