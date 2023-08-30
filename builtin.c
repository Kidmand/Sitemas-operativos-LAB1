#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>


#include "command.h"
#include "builtin.h"

static const char *internal_commands[] = {"cd", "echo", "exit"};


// Funciones de comandos internos:
static unsigned int internal_length(const char *internal_commands[]) // Devuelve el numero de comandos internos
{
    unsigned int length = 0;
    while (internal_commands[length] != NULL)
    {
        length++;
    }
    return length;
}

static bool is_internal(char *comando) // Devuelve true si es un comando interno
{
    bool b = false;

    unsigned int length = internal_length(internal_commands);

    for (unsigned int i = 0; i < length; i++)
    {
        if (strcmp(comando, internal_commands[i]) == 0)
        {
            b = true;
            i = length;
        }
    }
    return b;
}


// Funciones para la ejecucion de comandos internos: 
static void f_cd(scommand args){
    char *directory = scommand_to_string(args); 
    int result = chdir(directory);
    if (result < 0)
    {
        fprintf(stderr, "Error al cambiar el directorio de trabajo\n");
        exit(EXIT_FAILURE);
    }
}

static void f_echo(scommand args){
    char *ret = scommand_to_string(args); 
    printf("%s\n", ret);
}
 

// Funciones del TAD:
bool builtin_is_internal(scommand cmd){

    assert(!scommand_is_empty(cmd));

    char *comando = scommand_front(cmd);

    return is_internal(comando);
}

bool builtin_alone(pipeline p) {

    assert(!pipeline_is_empty(p));
    
    return pipeline_length(p) == 1 && builtin_is_internal(pipeline_front(p));
}

void builtin_run(scommand cmd) {

    assert(builtin_is_internal(cmd));

    char *comando = scommand_front(cmd);

    scommand_pop_front(cmd); // cmd solo contiene los argumentos ahora

    if (strcmp(comando, "cd") == 0) {
        f_cd(cmd);
    } else if (strcmp(comando, "echo") == 0) {
        f_echo(cmd);
    } else if (strcmp(comando, "exit") == 0) {
        exit(0);
    }
}