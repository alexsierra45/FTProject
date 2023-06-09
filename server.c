#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define MAX_SIZE_BUFFER 1024
#define TOK_DELIM " \t\r\n\a"
#define ERROR "\033[0;31mmy_ftp\033[0m"
#define HTTP_NOT_FOUND "HTTP/1.1 404 Not Found\r\n\r\n"
#define HTTP_BAD_REQUEST "HTTP/1.1 400 Bad Request\r\n\r\n"
#define HTTP_INTERNAL_ERROR "HTTP/1.1 500 Internal Server Error\r\n\r\n"
#define HTTP_FORBIDDEN "HTTP/1.1 403 Forbidden\r\n\r\n"

/// @brief Function that builds a sockaddr_in structure
/// @param server_ip Server IP
/// @param server_port Server port
/// @return sockaddr_in structure
struct sockaddr_in build_server_addr(char *server_ip, int server_port) {
    struct sockaddr_in server = {0};
    server.sin_family = AF_INET;
    server.sin_port = htons(server_port);
    inet_aton(server_ip, &server.sin_addr);
    return server;
}

/// @brief Function that creates a socket
/// @param port Port to bind the socket
/// @return Socket file descriptor
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

int send_file(char *path, int clientfd) {


    return 1;
}

int navigate(char *path, int clientfd, char *root_path) {


    return 1;
}

void *handle_client(int clientfd, char *root_path) {
    char buffer[MAX_SIZE_BUFFER];

    if (recv(clientfd, buffer, MAX_SIZE_BUFFER, 0) == -1) {
        perror("Error recv failed");
        char *response = HTTP_INTERNAL_ERROR;
        send(clientfd, response, strlen(response), 0);
        exit(EXIT_FAILURE);
    }

    char **args = split_line(buffer, TOK_DELIM);

    if (args[0] != NULL && strcmp(args[0], "GET") == 0 && args[1] != NULL) {
        char *path = path_browser_to_server(args[1], root_path);

        if (!navigate(path, clientfd, root_path) && !send_file(path, clientfd)) {
            char *response = HTTP_NOT_FOUND;
            send(clientfd, response, strlen(response), 0);
        }
        free(path);
    } else {
        char *response = HTTP_BAD_REQUEST;
        send(clientfd, response, strlen(response), 0);
    }

    close(clientfd);

    free(args);

    return NULL;
}