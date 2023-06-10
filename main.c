#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <pwd.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <asm-generic/errno.h>
#include <errno.h>

#include "server.h"
#include "utils.h"

#define MAX_SIZE_BUFFER 1024
#define GREEN "\033[0;32m"
#define RESET "\033[0m"
#define ERROR "\033[0;31mmy_ftp\033[0m"

_Noreturn void loop(int port, char *root_path) {

    int sock = create_server(port);

    while (1) {
        struct sockaddr_in client;
        int len = sizeof(client);
        int sock_client = accept(sock, (struct sockaddr *) &client, (socklen_t *) &len);

        if (sock_client == -1) {
            fprintf(stderr, "%s: accept failed\n", ERROR);
            continue;
        }

        struct Client client_s;
        client_s.sock_client = sock_client;
        client_s.root_path = (char *) malloc(MAX_SIZE_BUFFER);
        strcpy(client_s.root_path, root_path);

        pthread_t thread;
        if (pthread_create(&thread, NULL, handle_client, &client_s) != 0) {
            fprintf(stderr, "%s: error creating thread\n", ERROR);
            close(sock_client);
            continue;
        }
    }
}

int main(int argn, char *argv[]) {
    signal(SIGPIPE, SIG_IGN);

    int port;
    char *root_path;

    uid_t uid;

    uid = getuid();
    struct passwd *pw = getpwuid(uid);

    if (argv[1] == NULL) {
        port = 5000;
    } else {
        int user_port;
        user_port = string_to_positive_int(argv[1]);

        if (user_port == -1) {
            fprintf(stderr, "%s: the port is not valid\n", ERROR);
            exit(EXIT_FAILURE);
        } else {
            port = user_port;
        }
    }

    if (argv[1] == NULL || argv[2] == NULL) {
        root_path = pw->pw_dir;
    } else {
        DIR *user_dir;
        user_dir = opendir(argv[2]);

        if (user_dir) {
            root_path = argv[2];
        } else if (ENOENT == errno) {
            fprintf(stderr, "%s: directory doesn't exist\n", ERROR);
            exit(EXIT_FAILURE);
        } else {
            fprintf(stderr, "%s: error opening directory\n", ERROR);
            exit(EXIT_FAILURE);
        }

        closedir(user_dir);
    }

    char print_path[MAX_SIZE_BUFFER];
    char print_url[MAX_SIZE_BUFFER];

    sprintf(print_path, "%smy_ftp-path%s: %s", GREEN, RESET, root_path);
    sprintf(print_url, "%smy_ftp-url%s:  http://localhost:%d", GREEN, RESET, port);

    puts(print_path);
    puts(print_url);

    loop(port, root_path);
}