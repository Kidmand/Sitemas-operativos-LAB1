#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../command.h"

int main(int argc, char *argv[])
{
    printf("Esto es un test de command.c\n");
    scommand comand = scommand_new();
    printf("Es vacia : %d - El tamaño es: %u \n", scommand_is_empty(comand), scommand_length(comand));
    scommand_push_back(comand, strdup("ls"));
    printf("El primer elemento es: %s\n", scommand_front(comand));
    printf("Es vacia : %d - El tamaño es: %u \n", scommand_is_empty(comand), scommand_length(comand));

    scommand_pop_front(comand);
    printf("Es vacia : %d - El tamaño es: %u \n", scommand_is_empty(comand), scommand_length(comand));

    scommand_set_redir_in(comand, strdup("./in"));
    scommand_set_redir_out(comand, strdup("./out"));
    printf("La entrada es: %s\n", scommand_get_redir_in(comand));
    printf("La salida es: %s\n", scommand_get_redir_out(comand));

    scommand_push_back(comand, strdup("ls"));
    scommand_push_back(comand, strdup("-l"));
    scommand_push_back(comand, strdup("xd"));
    printf("El comando completo es : %s \n", scommand_to_string(comand));

    comand = scommand_destroy(comand);
    return EXIT_SUCCESS;
}
