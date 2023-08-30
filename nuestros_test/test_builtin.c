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
    printf("%s es un comando interno: %d \n", scommand_front(comand), b);

    scommand_pop_front(comand);
    scommand_push_back(comand, strdup("echo"));

    b = builtin_is_internal(comand);
    printf("%s es un comando interno: %d \n", scommand_front(comand), b);

    pipeline p = pipeline_new();
    pipeline_push_back(p,comand); 
    printf("El pipeline %s tiene solo un elemento y este se corresponde a un comando interno: %d \n",pipeline_to_string(p) ,builtin_alone(p));

    // Probamos echo
    scommand_pop_front(comand);
    scommand_push_back(comand, strdup("echo"));
    scommand_push_back(comand, strdup("hello word!!"));
    builtin_run(comand);


    // Probamos exit
    scommand_pop_front(comand);
    scommand_push_back(comand, strdup("exit"));
    printf("%s \n", scommand_front(comand));
    builtin_run(comand);

    exit(0); // no hace nada ¯⁠\⁠_⁠(⁠ツ⁠)⁠_⁠/⁠¯

    return EXIT_SUCCESS;
}