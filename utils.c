#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <curl/curl.h>

#define MAX_SIZE_BUFFER 1024

char **split_line(char *line, char *split) {
    int buf_size = MAX_SIZE_BUFFER, position = 0;
    char **tokens = malloc(buf_size * sizeof(char *));
    char *token;

    token = strtok(line, split);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= buf_size) {
            buf_size += MAX_SIZE_BUFFER;
            tokens = realloc(tokens, buf_size * sizeof(char *));
            if (!tokens) {
                fprintf(stderr, "allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, split);
    }
    tokens[position] = NULL;
    return tokens;
}

char *path_to_url(char *path) {
    char **args = split_line(path, "/");

    char *url = (char *) malloc(MAX_SIZE_BUFFER);
    url[0] = 0;

    if (args[0] == NULL) strcat(url, "/");

    for (int i = 0; args[i] != NULL; i++) {
        char *output;

        CURL *curl = curl_easy_init();

        output = curl_easy_escape(curl, args[i], (int) strlen(args[i]));

        strcat(url, "/");
        strcat(url, output);
        curl_easy_cleanup(curl);
        free(output);
    }

    free(args);

    return url;
}

char *url_to_path(char *path) {
    char *output;
    CURL *curl = curl_easy_init();

    int output_length;
    output = curl_easy_unescape(curl, path, (int) strlen(path), &output_length);
    curl_easy_cleanup(curl);

    return output;
}

// Convert a string to a positive integer
int string_to_positive_int(char *str) {
    int output = atoi(str);
    return output < 0 ? -1 : output;
}