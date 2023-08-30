#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "command.h"

static const char *internal_commands[] = {"cd", "echo", "exit"};

static unsigned int internal_length(const char *internal_commands){
    unsigned int length = 0;
    while (internal_commands[length] != NULL) {
        length++;
    }
    return length;
}

static bool is_internal(char *comando){
    bool b = false;

    unsigned int length = internal_length(internal_commands);

    for (unsigned int i = 0; i < length; i++)
    {
        if (*comando == internal_commands[i])
        {
            b = true;
            i = length;
        }
    }   
}

bool builtin_is_internal(scommand cmd){

    char *comando = !scommand_is_empty(cmd) ? scommand_front(cmd) : NULL;

    return is_internal(comando);
}

bool builtin_alone(pipeline p){}
/*
 * Indica si el pipeline tiene solo un elemento y si este se corresponde a un
 * comando interno.
 *
 * REQUIRES: p != NULL
 *
 * ENSURES:
 *
 * builtin_alone(p) == pipeline_length(p) == 1 &&
 *                     builtin_is_internal(pipeline_front(p))
 *
 *
 */

void builtin_run(scommand cmd){}
/*
 * Ejecuta un comando interno
 *
 * REQUIRES: {builtin_is_internal(cmd)}
 *
 */