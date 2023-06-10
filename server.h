#ifndef MY_FTP_SERVER_H
#define MY_FTP_SERVER_H

struct Client {
    int sock_client;
    char *root_path;
};

int create_server(int port);

void *handle_client(int sock_client, char *root_path);

#endif // MY_FTP_SERVER_H
