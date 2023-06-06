#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <pwd.h>
#include <errno.h>
#include <dirent.h>
#include <netinet/in.h>
#include <asm-generic/errno.h>
#include <utils.c>

#define MAX_SIZE_BUFFER 1024
#define BLUE "\033[0;34m"
#define RESET "\033[0m"
#define ERROR "\033[0;31mmy_ftp\033[0m"

// Función que ejecuta el loop principal
void loop(int port, char *root_path) {

}

// Función que asigna el puerto
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

// Función que asigna el directorio raíz
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

// Función principal
int main(int argn, char *argv[]) {
    signal(SIGPIPE, SIG_IGN);

    int port;
    char *root_path;

    // Asignación de puerto y directorio raíz
    asign_port(&port, argv);
    asign_root_path(&root_path, argv);
    
    char print_path[MAX_SIZE_BUFFER];
    char print_url[MAX_SIZE_BUFFER];

    sprintf(print_path, "%smy_ftp-path%s: %s", BLUE, RESET, root_path);
    sprintf(print_url, "%smy_ftp-url%s:  http://localhost:%d", BLUE, RESET, port);

    puts(print_path);
    puts(print_url);

    loop(port, root_path);
}