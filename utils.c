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

// Convert a string to a positive integer
int string_to_positive_int(char *str) {
    int output = atoi(str);
    return output < 0 ? -1 : output;
}