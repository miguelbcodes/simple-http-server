#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>

#define PORT 8080
#define BUFFER_SIZE 1024

typedef struct {
  char method[16];
  char url[256];
  char protocol[16];
} http_request;

int create_socket();
void bind_socket(int server_fd, struct sockaddr_in *address);
void listen_socket(int server_fd);
void accept_connection(int server_fd);
void parse_http_request(const char *request, http_request *parsed_request);
void generate_response(int client_socket, const char *status, const char *content_type, const char *body);
void serve_static_file(int client_socket, const char *path);
void *handle_client(void *client_socket);

#endif
