#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "execute.h"
#include "command.h"
#include "builtin.h"

void execute_pipeline(pipeline apipe)
{
    // reconocemos entre comandos internos y externos
    scommand sc = pipeline_front(apipe);

    bool is_internal = builtin_is_internal(sc);

    if (is_internal) // función interna
    {
        builtin_run(sc);
    }
    else
    {
        printf("Es un comando externo, todavia no esta implementado.\n");
    }
}