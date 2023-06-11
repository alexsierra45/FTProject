#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <errno.h>

#include "operations.c"

struct sockaddr_in build_server_addr(char *server_ip, int server_port) {
    struct sockaddr_in server = {0};
    server.sin_family = AF_INET;
    server.sin_port = htons(server_port);
    inet_aton(server_ip, &server.sin_addr);
    return server;
}

int create_server(int port) {
    struct sockaddr_in server;
    int socketfd;

    socketfd = socket(AF_INET, SOCK_STREAM, 0);

    if (socketfd == -1) {
        perror("Error socket creation failed");
        exit(EXIT_FAILURE);
    }

    setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int));
    server = build_server_addr("localhost", port);

    if (bind(socketfd, (struct sockaddr *) &server, sizeof(server)) == -1) {
        perror("Error binding failed");
        exit(EXIT_FAILURE);
    }

    if (listen(socketfd, 1) == -1) {
        perror("Error listen failed");
        exit(EXIT_FAILURE);
    }

    return socketfd;
}