#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "server.h"

int main() {
  int server_fd;
  struct sockaddr_in address;

  server_fd = create_socket();

  printf("Socket created successfully\n");

  return 0;
}