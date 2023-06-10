#ifndef MY_FTP_UTILS_H
#define MY_FTP_UTILS_H


char *path_to_url(char *path);

char **split_line(char *line,char *split);

/**
 * Add the root path to the path
 */
char *path_browser_to_server(char *path, char *root_path);

/**
 * Remove the root path from the path
 */
char *path_server_to_browser(char *path, char *root_path);

/**
 * Remove the last directory from a path
 * @param path
 */
void back_path(char *path);

/**
 * Convert a string to a positive integer
 * @param str
 * @return
 * -1 if the string is not a number or the number is not valid
 */
int string_to_positive_int(char *str);

#endif //MY_FTP_UTILS_H
