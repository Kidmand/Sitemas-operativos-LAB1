#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "../command.h"

int main(int argc, char *argv[])
{
    printf("Esto es un test de command.c\n");
    scommand comand = scommand_new();
    scommand_push_back(comand, "ramiro");
    comand = scommand_destroy(comand);
    return EXIT_SUCCESS;
}
