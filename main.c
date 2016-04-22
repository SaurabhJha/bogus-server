#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT "3490"
#define BACKLOG 5

int main(int argc, char *argv[])
{
  struct addrinfo hints, *res;
  char *message;
  struct sockaddr *server_address, *client_address;
  int server_socket, client_socket, yes = 1, server_address_len,
    client_address_size;

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  getaddrinfo(NULL, PORT, &hints, &res);
  server_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  server_address = res->ai_addr;
  server_address_len = res->ai_addrlen;
  bind(server_socket, server_address, server_address_len);
  listen(server_socket, BACKLOG);
  client_address = NULL;
  client_address_size = sizeof(client_address);
  while (1) {
    client_socket = accept(server_socket, client_address, &client_address_size);
    message = "Hello, World\n";
    send(client_socket, message, strlen(message), 0);
    close(client_socket);
  }

  return 0;
}
