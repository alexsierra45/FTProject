

// Convert a string to a positive integer
int string_to_positive_int(char *str) {
    int output = atoi(str);
    return output < 0 ? -1 : output;
}