#ifndef SERVER_H
#define SERVER_H

#include <netinet/in.h>

#define PORT 8080

int create_socket();
void bind_socket(int server_fd, struct sockaddr_in *address);

#endif
