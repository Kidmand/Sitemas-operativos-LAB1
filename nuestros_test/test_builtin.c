#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../command.h"
#include "../builtin.h"

int main(int argc, char *argv[])
{
    scommand comand = scommand_new();
    scommand_push_back(comand, strdup("ls"));

    bool b = builtin_is_internal(comand);
    printf("%s es un comando interno: %d \n",scommand_front(comand), b);

    scommand_pop_front(comand);
    scommand_push_back(comand, strdup("echo"));

    b = builtin_is_internal(comand);
    printf("%s es un comando interno: %d \n",scommand_front(comand), b);

    return EXIT_SUCCESS;
}