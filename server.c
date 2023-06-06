#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

struct sockaddr_in build_server_addr(char *server_ip, int server_port) {
    struct sockaddr_in server = {0};
    server.sin_family = AF_INET;
    server.sin_port = htons(server_port);
    inet_aton(server_ip, &server.sin_addr);
    return server;
}

int create_socket(int port) {
    struct sockaddr_in server_address;
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }
    
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int));
    server_address = build_server_addr("localhost", port);

    if (bind(sockfd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 1) == -1) {
        perror("Error listening socket");
        exit(EXIT_FAILURE);
    }

    return sockfd;
}