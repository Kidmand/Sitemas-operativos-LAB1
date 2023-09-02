#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "execute.h"
#include "command.h"
#include "builtin.h"

static void print_execute_error(char *message)
{
    printf("ERROR en el execute: %s\n", message);
}

static void extern_run(scommand cmd)
{
    assert(cmd != NULL);
    // Obtengo los argumentos
    char **argv = scommand_to_argv(cmd);

    execvp(argv[0], argv); // Ejecuta el proceso.

    // Si sigue ejecutando es porque fallo

    print_execute_error("Fallo la ejecucion del proceso.");
}

static void execute_single_cmd(scommand cmd)
{
    assert(cmd != NULL);
    bool is_internal = builtin_is_internal(cmd);
    if (is_internal)
    {
        builtin_run(cmd);
    }
    else
    {
        extern_run(cmd);
    }
}

void execute_pipeline(pipeline apipe)
{
    assert(apipe != NULL);
    scommand cmd;
    // bool is_background = pipeline_get_wait(apipe);

    while (pipeline_length(apipe) > 0)
    {
        pid_t pc_id_fork = fork();
        if (pc_id_fork == 0)
        {
            cmd = pipeline_front(apipe);
            execute_single_cmd(cmd);
            pipeline_pop_front(apipe);
        }
        if (pc_id_fork > 0)
        {
            pid_t wait_response = wait(NULL);
            if (wait_response == -1)
                print_execute_error("Ocurrio un error al ejecutar el wait()\n");
        }
        else
        {
            print_execute_error("Ocurrio un error al ejecutar el fork()\n");
        }
    }
}