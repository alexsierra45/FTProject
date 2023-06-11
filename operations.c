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

// Download a file
int download_file(char *path, int socketfd) {
    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        return 0;
    }

    struct stat stat_buf;
    if (fstat(fd, &stat_buf) == -1) {
        perror("Error getting file status");
        char *response = HTTP_INTERNAL_ERROR;
        send(socketfd, response, strlen(response), 0);
        return 1;
    }

    if ((stat_buf.st_mode & S_IRUSR) != S_IRUSR) {
        perror("Error file doesn't have read permission");
        char *response = HTTP_FORBIDDEN;
        send(socketfd, response, strlen(response), 0);
        return 1;
    }

    char header[MAX_SIZE_BUFFER];
    snprintf(header, sizeof(header), "HTTP/1.1 200 OK\r\n"
                                     "Content-Type: application/octet-stream\r\n"
                                     "Content-Disposition: attachment; filename=\"%s\"\r\n"
                                     "Content-Length: %ld\r\n"
                                     "\r\n", path, stat_buf.st_size);

    if (send(socketfd, header, strlen(header), 0) == -1 || 
        sendfile(socketfd, fd, 0, stat_buf.st_size) == -1) {
            perror("Error send failed");
            char *response = HTTP_INTERNAL_ERROR;
            send(socketfd, response, strlen(response), 0);
    }

    close(fd);
    return 1;
}

// Send a directory to the client
int navigate(char *path, int socketfd, char *root_path) {
    DIR *dir;
    dir = opendir(path);
    if (dir == NULL) {
        if (errno != EACCES) return 0;
        send(socketfd, HTTP_FORBIDDEN, strlen(HTTP_FORBIDDEN), 0);
        perror("Error access denied");
        return 1;
    }

    char *response = render(dir, path, root_path);
    if (send(socketfd, response, strlen(response), 0) == -1) {
        perror("Error send file");
        send(socketfd, HTTP_INTERNAL_ERROR, strlen(HTTP_INTERNAL_ERROR), 0);
        return 1;
    }
    free(response);
    closedir(dir);
    return 1;
}

void *handle_client(int socketfd, char *root_path) {
    char buffer[MAX_SIZE_BUFFER];

    if (recv(socketfd, buffer, MAX_SIZE_BUFFER, 0) == -1) {
        perror("Error recv");
        char *response = HTTP_INTERNAL_ERROR;
        send(socketfd, response, strlen(response), 0);
        exit(1);
    }

    char **request = split_line(buffer, TOK_DELIM);

    if (request[0] != NULL && strcmp(request[0], "GET") == 0 && request[1] != NULL) {
        char *path = path_browser_to_server(request[1], root_path);

        if (!navigate(path, socketfd, root_path) && !download_file(path, socketfd)) {
            char *response = HTTP_NOT_FOUND;
            send(socketfd, response, strlen(response), 0);
        }
        free(path);
    } else {
        char *response = HTTP_BAD_REQUEST;
        send(socketfd, response, strlen(response), 0);
    }
    close(socketfd);
    free(request);
    return NULL;
}