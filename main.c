#include <unistd.h>
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

#include "server.c"

#define MAX_SIZE_BUFFER 1024
#define BLUE "\033[0;34m"
#define RESET "\033[0m"
#define ERROR "\033[0;31mmy_ftp\033[0m"
#define True 1

void loop(int port, char *root_path) {

    int sock = create_server(port);

    while (True) {
        struct sockaddr_in client;
        int len = sizeof(client);
        int sock_client = accept(sock, (struct sockaddr *) &client, (socklen_t *) &len);

        if (sock_client == -1) {
            perror("Error accept failed");
            continue;
        }

        int pid = fork();

        if (pid < 0) {
            perror("Error forking");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0) {
            close(sock);
            handle_client(sock_client, root_path);
            exit(EXIT_SUCCESS);
        }
        else close(sock_client);
    }
}

// Function that asign the port
void asign_port(int *port, char *args[]) {
    if (args[1] == NULL) *port = 5000;
    else {
        int user_port;
        user_port = string_to_positive_int(args[1]);

        if (user_port == -1) {
            fprintf(stderr, "%s: the port is not valid\n", ERROR);
            exit(EXIT_FAILURE);
        } else {
            *port = user_port;
        }
    }
}

// Function that assigns the root path
void asign_root_path(char **root_path, char *args[]) {
    uid_t uid;
    uid = getuid();
    struct passwd *pw = getpwuid(uid);

    if (args[1] == NULL || args[2] == NULL) {
        *root_path = pw->pw_dir;
    } else {
        DIR *user_dir;
        user_dir = opendir(args[2]);

        if (user_dir) *root_path = args[2];
        else {
            fprintf(stderr, "%s: Directory does not exist\n", ERROR);
            exit(EXIT_FAILURE);
        }

        closedir(user_dir);
    }
}

// Main function
int main(int argn, char *argv[]) {
    signal(SIGPIPE, SIG_IGN);

    int port;
    char *root_path;

    // Port and root path asignation
    asign_port(&port, argv);
    asign_root_path(&root_path, argv);
    
    char print_path[MAX_SIZE_BUFFER];
    char print_url[MAX_SIZE_BUFFER];

    sprintf(print_path, "%spath%s: %s", BLUE, RESET, root_path);
    sprintf(print_url, "%surl%s: http://localhost:%d", BLUE, RESET, port);

    puts(print_path);
    puts(print_url);

    loop(port, root_path);
}