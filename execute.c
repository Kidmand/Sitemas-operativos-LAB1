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

/* Funcion para los mensajes de error del wait */
static void wait_(void)
{
    pid_t wait_response_for_background = wait(NULL);
    if (wait_response_for_background == -1)
        print_execute_error("Ocurrio un error al ejecutar el wait()\n");
}

/* Funcion encargada de ejecutar programas externos*/
static void scommand_external_exec(scommand cmd)
{
    assert(cmd != NULL);

    // Obtengo los argumentos
    char **argv = scommand_to_argv(cmd);

    execvp(argv[0], argv); // Ejecuta el proceso.

    // Si sigue ejecutando es porque fallo
    print_execute_error("Fallo la ejecucion del proceso.");
}

/* Funcion encargada de ejecutar un comando*/
static void scommand_exec(scommand cmd)
{
    assert(cmd != NULL);

    if (builtin_is_internal(cmd))
    {
        builtin_run(cmd);
        exit(EXIT_SUCCESS);
    }
    else if (!scommand_is_empty(cmd))
    {
        scommand_external_exec(cmd);
    }
    else
    {
        exit(EXIT_SUCCESS);
    }

    // No deberia llegar aca
    assert(false);
}

/* Funcion encargada de ejecutar un comando solo */
static unsigned int execute_command_single(pipeline apipe)
{
    scommand cmd = pipeline_front(apipe);

    unsigned int active_child_processes = 0u;

    if (builtin_is_internal(cmd))
    {
        builtin_run(cmd);
    }
    else
    {
        pid_t conection = fork();

        if (conection == 0)
        {
            scommand_exec(cmd);
        }
        else if (conection > 0)
        {
            active_child_processes++;
        }
        else
        {
            print_execute_error("Error al ejecutar el fork\n");
            return active_child_processes;
        }
    }

    return active_child_processes;
}

/* Funcion encargada de ejecutar un pipeline de dos elementos*/
static unsigned int execute_command_multipe(pipeline apipe)
{
    assert(apipe != NULL && pipeline_length(apipe) >= 2u);

    unsigned int active_child_processes = 0u;

    unsigned int numberOfPipes = pipeline_length(apipe) - 1u;

    bool error_flag = false;

    // Se crean los pipes
    int pipesfd[2];

    // Se abre el pipe
    int res_pipe = pipe(pipesfd);
    if (res_pipe < 0)
    {
        print_execute_error("Fallo el pipe");
        // se cierran los pipes que ya se abrieron para no generar bugs
        close(pipesfd[0]);
        close(pipesfd[1]);
        return active_child_processes;
    }

    unsigned int j = 0;

    // Se generan los forks necesarios
    while (!pipeline_is_empty(apipe) && !error_flag)
    {
        pid_t pid = fork();
        if (pid < 0)
        {
            perror("error");
            error_flag = true; // Se corta el bucle
        }
        if (pid == 0)
        {
            // Si no es el ultimo comando
            if (pipeline_length(apipe) > 1)
            {
                int res_dup = dup2(pipesfd[j + 1], STDOUT_FILENO);
                if (res_dup < 0)
                {
                    perror("dup");
                    _exit(EXIT_FAILURE);
                }
            }

            // Si no es el primer comando
            if (j != 0u)
            {
                int res_dup = dup2(pipesfd[j - 2u], STDIN_FILENO);
                if (res_dup < 0)
                {
                    perror("dup");
                    _exit(EXIT_FAILURE);
                }
            }

            // Se cierran todos los file descriptors que se usaron
            for (unsigned int i = 0u; i < 2u * numberOfPipes; i++)
            {
                close(pipesfd[i]);
            }

            // Se ejecuta el comando
            scommand_exec(pipeline_front(apipe));
        }
        else if (pid > 0)
        {
            // Elimina un comando del pipe y aumenta el contador de procesos hijo
            pipeline_pop_front(apipe);
            j = j + 2u;
            active_child_processes++;
        }
    }

    // Se cierran los descriptores de archivo por completo
    for (unsigned int i = 0u; i < 2u * numberOfPipes; i++)
    {
        close(pipesfd[i]);
    }

    return active_child_processes;
}

/* Funcion encargada de selecionar el modo de ejecucion de un pipeline*/
static unsigned int select_mode_pipline(pipeline apipe)
{
    unsigned int active_child_processes = 0u;
    if (pipeline_length(apipe) == 1)
    {
        active_child_processes = execute_command_single(apipe); // Ejecuta el comando, ya sea interno o externo si no existe un pipe (|)
    }
    else if (pipeline_length(apipe) == 2)
    {
        active_child_processes = execute_command_multipe(apipe);
    }
    else
    {
        print_execute_error("La terminal solo toma 2 comandos como maximo");
    }
    return active_child_processes;
}

/* Funcion encargada de ejecutar un pipeline en su totalidad*/
void execute_pipeline(pipeline apipe)
{
    assert(apipe != NULL);
    if (pipeline_get_wait(apipe))
    {
        unsigned int active_child_processes = select_mode_pipline(apipe);

        // Se espera a que todos los hijos terminen
        while (active_child_processes > 0u)
        {
            wait_();
            active_child_processes--;
        }
    }
    else
    {
        // Hay que correrlo en modo background
        pid_t pid = fork();
        if (pid < 0)
        {
            // Caso de que el fork falle
            perror("fork");
        }
        else if (pid == 0)
        {
            // El proceso hijo

            // Se conecta el stdin del hijo a un archivo vacio
            /* Como archivo vacio se usa una punta de lectura de pipe
               con punta de escritura cerrada */
            int pipefds[2];
            int res_pipe = pipe(pipefds);
            if (res_pipe < 0)
            {
                perror("pipe");
                exit(EXIT_FAILURE);
            }
            int punta_lectura = pipefds[0];
            int punta_escritura = pipefds[1];

            close(punta_escritura);

            int res_dup2 = dup2(punta_lectura, STDIN_FILENO);
            if (res_dup2 < 0)
            {
                perror("perror ");
                exit(EXIT_FAILURE);
            }

            // Ejecuta todos los comandos del pipeline
            select_mode_pipline(apipe);

            // Y termina para que los hijos pasen a ser hijos del sistema
            exit(EXIT_SUCCESS);
        }
        else
        {
            // El proceso padre
            // Espera a que el hijo termine de crear todos los procesos
            wait(NULL);
        }
    }
}

/*
FALTAN MUCHAS COSAS:
 - Implementar la funcionalidad del pipe (Se tiene  que pasar el stdout al stdin del siguinte comando) (HECHO)
 - Implementar el manejo de in/out  de archivos (No esta hecho nada)
 - Arreglar CTL+D (HECHO)
*/