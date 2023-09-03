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

/* Funcion para los mensajes de error */
static void print_execute_error(char *message)
{
    printf("ERROR en el execute: %s\n", message);
}

/* Funcion encargada de ejecutar programas externos*/
static void external_run(scommand cmd)
{
    assert(cmd != NULL);

    // Obtengo los argumentos
    char **argv = scommand_to_argv(cmd);

    execvp(argv[0], argv); // Ejecuta el proceso.

    // Si sigue ejecutando es porque fallo
    print_execute_error("Fallo la ejecucion del proceso.");
}

void execute_pipeline(pipeline apipe)
{
    assert(apipe != NULL);
    scommand cmd;
    bool is_foreground = pipeline_get_wait(apipe);

    pid_t pc_id_for_background = fork(); // Creamos un nuevo proceso para ejecutar todos los comandos.

    if (pc_id_for_background == 0) // El hijo ejecuta la totalidad de los comandos
    {
        // Iteramos por todos los comandos
        while (pipeline_length(apipe) > 0)
        {
            cmd = pipeline_front(apipe); // Obtenermos el primer comando del pipline

            // En el caso que el comando builtin eso se va a ejecutar
            if (builtin_is_internal(cmd))
            {
                builtin_run(cmd);
                pipeline_pop_front(apipe);
            }
            else
            {
                pid_t pc_id_fork = fork(); // Creamos un nuevo proceso para cada uno de los comandos
                if (pc_id_fork == 0)       // En el hijo ejecutamos un solo comando
                {
                    external_run(cmd); // Ejecutamos el comando simple externo
                }
                else if (pc_id_fork > 0) // El padre, encargado de esperar al hijo siempre
                {
                    pid_t wait_response = wait(NULL);
                    if (wait_response == -1) // Manejo de errores
                        print_execute_error("Ocurrio un error al ejecutar el wait()\n");
                    pipeline_pop_front(apipe);
                }
                else // Manejo de errores
                {
                    print_execute_error("Ocurrio un error al ejecutar el segundo fork()\n");
                }
            }
        }
    }
    else if (pc_id_for_background > 0) // El padre es el encargado de decidir si espera o no al hijo
    {
        if (is_foreground) // Se fija si el pipline se ejecuta en foreground o no
        {
            pid_t wait_response_for_background = wait(NULL);
            if (wait_response_for_background == -1)
                print_execute_error("Ocurrio un error al ejecutar el wait()\n");
        }
    }
    else // Manejo de errores
    {
        print_execute_error("Ocurrio un error al ejecutar el primer fork()\n");
    }
}

/*
FALTAN MUCHAS COSAS:
 - Implementar la funcionalidad del pipe (Se tiene  que pasar el stdout al stdin del siguinte comando)
 - Implementar el manejo de in/out  de archivos (No esta echo nada)
 - Arreglar CTL+D (Nose porque no anda despues de ejecutar cosas)
*/