

// Función que convierte una cadena a un entero positivo
int string_to_positive_int(char *str) {
    int output = atoi(str);
    return output < 0 ? -1 : output;
}