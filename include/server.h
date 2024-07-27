#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int create_socket();
void bind_socket(int server_fd, struct sockaddr_in *address);
void listen_socket(int server_fd);
void accept_connection(int server_fd);
void parse_http_request(const char *request);

#endif
