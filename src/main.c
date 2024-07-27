#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "server.h"

int main() {
  int server_fd;
  struct sockaddr_in address;

  server_fd = create_socket();
  bind_socket(server_fd, &address);

  printf("Socket bound successfully\n");

  return 0;
}