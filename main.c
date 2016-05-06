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
  struct sockaddr *client_address;
  socklen_t *client_address_len;
  int client_socket;
  char *message;

  register_signal_handlers();
  server_socket = create_tcp_listener();
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
