#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BACKLOG 4
#define PORT 3000

int main()
{
  int s, c, b;
  struct sockaddr_in sa;
  FILE *client;

  bzero(&sa, sizeof(sa));
  sa.sin_family = AF_INET;
  sa.sin_port = htons(PORT);
  
  s = socket(AF_INET, SOCK_STREAM, 0);
  if (INADDR_ANY)
    sa.sin_addr.s_addr = htonl(INADDR_ANY);
  bind(s, (struct sockaddr *) &sa, sizeof(sa));
  listen(s, BACKLOG);
  b = sizeof(sa);

  while (1) {
    c = accept(s, (struct sockaddr *) &sa, &b);
    client = fdopen(c, "w");
    fprintf(client, "This server is working.\n");
    fclose(client);
  }
  return 0;
}
