#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include "server.h"

int create_socket() {
  int server_fd;
  int opt = 1;

  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }

  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
    perror("setsockopt");
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  return server_fd;
}

void bind_socket(int server_fd, struct sockaddr_in *address) {
  address->sin_family = AF_INET;
  address->sin_addr.s_addr = INADDR_ANY;
  address->sin_port = htons(PORT);

  if (bind(server_fd, (struct sockaddr *)address, sizeof(*address))) {
    perror("bind failed");
    close(server_fd);
    exit(EXIT_FAILURE);
  }
}

void listen_socket(int server_fd) {
  if (listen(server_fd, 3) < 0) {
    perror("listen");
    close(server_fd);
    exit(EXIT_FAILURE);
  }
}

void accept_connection(int server_fd) {
  int new_socket;
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  char buffer[BUFFER_SIZE] = {0};
  ssize_t valread;

  if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
    perror("accept");
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  valread = read(new_socket, buffer, BUFFER_SIZE);
  if (valread < 0) {
    perror("read");
    close(new_socket);
    close(server_fd);
    exit(EXIT_FAILURE);
  }

  printf("Received request:\n%s\n", buffer);

  parse_http_request(buffer);

  close(new_socket);
}

void parse_http_request(const char *request) {
  char method[16];
  char url[256];
  char protocol[16];

  sscanf(request, "%s %s %s", method, url, protocol);

  printf("Method: %s\n", method);
  printf("URL: %s\n", url);
  printf("Protocol: %s\n", protocol);
}

void generate_response(int client_socket, const char *status, const char *content_type, const char *body) {
  char response[BUFFER_SIZE];
  snprintf(response, sizeof(response),
          "HTTP/1.1 %s\n"
          "Content-Type: %s\n"
          "Content-Length: %zu\n"
          "\n"
          "%s",
          status, content_type, strlen(body), body);
  
  write(client_socket, response, strlen(response));
}