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

#include "render.c"

#define MAX_SIZE_BUFFER 1024
#define TOK_DELIM " \t\r\n\a"
#define ERROR "\033[0;31mmy_ftp\033[0m"
#define HTTP_NOT_FOUND "HTTP/1.1 404 Not Found\r\n\r\n"
#define HTTP_BAD_REQUEST "HTTP/1.1 400 Bad Request\r\n\r\n"
#define HTTP_INTERNAL_ERROR "HTTP/1.1 500 Internal Server Error\r\n\r\n"
#define HTTP_FORBIDDEN "HTTP/1.1 403 Forbidden\r\n\r\n"

struct sockaddr_in build_server_addr(char *server_ip, int server_port) {
    struct sockaddr_in server = {0};
    server.sin_family = AF_INET;
    server.sin_port = htons(server_port);
    inet_aton(server_ip, &server.sin_addr);
    return server;
}

int create_server(int port) {
    struct sockaddr_in server;
    int sock1;

    sock1 = socket(AF_INET, SOCK_STREAM, 0);

    if (sock1 == -1) {
        perror("Error socket creation failed");
        exit(EXIT_FAILURE);
    }

    setsockopt(sock1, SOL_SOCKET, SO_REUSEADDR, &(int) {1}, sizeof(int));
    server = build_server_addr("localhost", port);

    if (bind(sock1, (struct sockaddr *) &server, sizeof(server)) == -1) {
        perror("Error binding failed");
        exit(EXIT_FAILURE);
    }

    if (listen(sock1, 1) == -1) {
        perror("Error listen failed");
        exit(EXIT_FAILURE);
    }

    return sock1;
}

/**
 * @brief Send a directory to the client, 403 if the directory is not accessible, 500 if an error occurred
 * @param path Path to the directory
 * @param sock_client Socket to the client
 * @param root_path Path to the root directory
 * @return
 * 0 if the directory was not found\n
 * 1 if was sent a response to the client
 */
int navigate(char *path, int sock_client, char *root_path) {
    DIR *dir;
    dir = opendir(path);
    if (dir == NULL) {
        if (errno != EACCES) return 0;
        send(sock_client, HTTP_FORBIDDEN, strlen(HTTP_FORBIDDEN), 0);
        perror("Error access denied");
        return 1;
    }

    char *response = render(dir, path, root_path);
    if (send(sock_client, response, strlen(response), 0) == -1) {
        perror("Error send file");
        send(sock_client, HTTP_INTERNAL_ERROR, strlen(HTTP_INTERNAL_ERROR), 0);
        return 1;
    }
    free(response);
    closedir(dir);
    return 1;
}

/**
 * @brief Send a file to the client, 403 if the file haven't read permission, 500 if an error occurred
 * @param path Path to the file
 * @param sock_client Socket to the client
 * @return 0 if the file was not found\n
 * 1 if was sent a response to the client
 */
int send_file(char *path, int sock_client) {
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        return 0;
    }

    struct stat stat_buf;
    if (fstat(fd, &stat_buf) == -1) {
        perror("Error getting file status");
        char *response = HTTP_INTERNAL_ERROR;
        send(sock_client, response, strlen(response), 0);
        return 1;
    }

    if ((stat_buf.st_mode & S_IRUSR) != S_IRUSR) {
        perror("Error file doesn't have read permission");
        char *response = HTTP_FORBIDDEN;
        send(sock_client, response, strlen(response), 0);
        return 1;
    }

    char header[MAX_SIZE_BUFFER];
    snprintf(header, sizeof(header), "HTTP/1.1 200 OK\r\n"
                                     "Content-Type: application/octet-stream\r\n"
                                     "Content-Disposition: attachment; filename=\"%s\"\r\n"
                                     "Content-Length: %ld\r\n"
                                     "\r\n", path, stat_buf.st_size);

    if (send(sock_client, header, strlen(header), 0) == -1 || 
        sendfile(sock_client, fd, 0, stat_buf.st_size) == -1) {
            perror("Error send failed");
            char *response = HTTP_INTERNAL_ERROR;
            send(sock_client, response, strlen(response), 0);
    }

    close(fd);
    return 1;
}

void *handle_client(int sock_client, char *root_path) {
    char buffer[MAX_SIZE_BUFFER];

    if (recv(sock_client, buffer, MAX_SIZE_BUFFER, 0) == -1) {
        perror("Error recv");
        char *response = HTTP_INTERNAL_ERROR;
        send(sock_client, response, strlen(response), 0);
        exit(1);
    }

    char **request = split_line(buffer, TOK_DELIM);

    if (request[0] != NULL && strcmp(request[0], "GET") == 0 && request[1] != NULL) {
        char *path = path_browser_to_server(request[1], root_path);

        if (!navigate(path, sock_client, root_path) && !send_file(path, sock_client)) {
            char *response = HTTP_NOT_FOUND;
            send(sock_client, response, strlen(response), 0);
        }
        free(path);
    } else {
        char *response = HTTP_BAD_REQUEST;
        send(sock_client, response, strlen(response), 0);
    }

    close(sock_client);

    free(request);

    return NULL;
}