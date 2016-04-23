#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

#define BACKLOG 5
#define PORT "3490"


int main(int argc, char *argv[])
{
  struct addrinfo hints, *res, *p;
  struct sockaddr *client_address;
  struct sockaddr_in *client_address_in;
  socklen_t *client_address_len;
  int status, server_socket, client_socket, yes = 1;
  char *message, client_ip_string[INET6_ADDRSTRLEN];

  memset(&hints, 0, sizeof(hints));

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if ((status = getaddrinfo(NULL, PORT, &hints, &res)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    return 2;
  }

  server_socket = 0;
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
    message = "Howdy?\n"; // What if this message is dynamic?
    send(client_socket, message, strlen(message), 0);
    close(client_socket);
    free(client_address);
    free(client_address_len);
  }

  close(server_socket);
  return 0;
}
