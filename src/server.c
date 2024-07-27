#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>
#include "server.h"

int create_socket() {
  int server_fd;
  int opt = 1;

  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
    perror("Socket creation failed");
    exit(EXIT_FAILURE);
  }

  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
    perror("Set socket options failed");
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
    perror("Binding socket failed");
    close(server_fd);
    exit(EXIT_FAILURE);
  }
}

void listen_socket(int server_fd) {
  if (listen(server_fd, 3) < 0) {
    perror("Listening on socket failed");
    close(server_fd);
    exit(EXIT_FAILURE);
  }
}

void accept_connection(int server_fd) {
  int new_socket;
  struct sockaddr_in address;
  int addrlen = sizeof(address);

  while(1) {
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
      perror("accept");
      continue;
    }

    pthread_t thread_id;
    int *client_socket = malloc(sizeof(int));
    *client_socket = new_socket;

    if (pthread_create(&thread_id, NULL, handle_client, client_socket) != 0) {
      perror("pthread_create");
      close(new_socket);
      free(client_socket);
    }

    pthread_detach(thread_id);
  }
}

void parse_http_request(const char *request, http_request *parsed_request) {
  sscanf(request, "%s %s %s", parsed_request->method, parsed_request->url, parsed_request->protocol);

  printf("Method: %s\n", parsed_request->method);
  printf("URL: %s\n", parsed_request->url);
  printf("Protocol: %s\n", parsed_request->protocol);
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

void serve_static_file(int client_socket, const char *path) {
  char full_path[256];
  char buffer[BUFFER_SIZE];
  struct stat st;
  int fd;
  ssize_t bytes_read;

  snprintf(full_path, sizeof(full_path), "static%s", path);
  if (stat(full_path, &st) == -1 || S_ISDIR(st.st_mode)) {
    generate_response(client_socket, "404 Not Found", "text/html", "<html><body><h1>404 Not Found</h1></body></html>");
    return;
  }

  fd = open(full_path, O_RDONLY);
  if (fd == -1) {
    generate_response(client_socket, "500 Internal Server Error", "text/html", "<html><body><h1>500 Internal Server Error</h1></body></html>");
    return;
  }

  char *content_type = "text/plain";
  if (strstr(full_path, ".html")) {
    content_type = "text/html";
  } else if (strstr(full_path, ".css")) {
    content_type = "text/css";
  } else if (strstr(full_path, ".js")) {
    content_type = "application/javascript";
  } else if (strstr(full_path, ".png")) {
    content_type = "image/png";
  } else if (strstr(full_path, ".jpg") || strstr(full_path, ".jpeg")) {
    content_type = "image/jpeg";
  }

  snprintf(buffer, sizeof(buffer), "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %ld\r\n\r\n", content_type, st.st_size);
  write(client_socket, buffer, strlen(buffer));

  while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
    write(client_socket, buffer, bytes_read);
  }

  close(fd);
}

void *handle_client(void *client_socket) {
  int socket = *((int *)client_socket);
  free(client_socket);
  char buffer[BUFFER_SIZE] = {0};
  ssize_t valread;

  valread = read(socket, buffer, BUFFER_SIZE);
  if (valread < 0) {
    perror("read");
    close(socket);
    return NULL;
  }

  printf("Recieved request: \n%s\n", buffer);

  http_request parsed_request;
  parse_http_request(buffer, &parsed_request);

  if (strcmp(parsed_request.method, "GET") == 0) {
    serve_static_file(socket, parsed_request.url);
  } else {
    generate_response(socket, "405 Method Not Allowed", "text/html", "<html><body><h1>405 Method Not Allowed</h1></body></html>");
  }

  close(socket);
  return NULL;
}