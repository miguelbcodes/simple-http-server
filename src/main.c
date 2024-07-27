#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "server.h"

int main() {
  int server_fd;
  struct sockaddr_in address;

  server_fd = create_socket();
  bind_socket(server_fd, &address);
  listen_socket(server_fd);

  printf("Server is listening on port %d\n", PORT);

  while(1) {
    accept_connection(server_fd);
  }

  return 0;
}