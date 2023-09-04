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

    // Se crean los pipes
    int fd[2];

    // Se abre el pipe
    int res_pipe = pipe(fd);
    if (res_pipe < 0)
    {
        print_execute_error("Fallo el pipe");
        // se cierran los pipes que ya se abrieron para no generar bugs
        close(fd[0]);
        close(fd[1]);
        return active_child_processes;
    }

    // Se obtienen los file descriptors para mejor legibilidad
    int fd_read = fd[0];
    int fd_write = fd[1];
    // Se generan el fork para el primer comando
    pid_t pid_first = fork();
    if (pid_first < 0)
    {
        print_execute_error("Error con el fork");
    }
    if (pid_first == 0)
    {
        // Como no es el ultimo comando
        close(fd_read); // cierra el extremo de lectura del pipe (tuberia), se lo conoce como read
        int res_dup = dup2(fd_write, STDOUT_FILENO);
        if (res_dup < 0)
        {
            print_execute_error("Error con el dup");
            _exit(EXIT_FAILURE);
        }

        // Se cierran todos los file descriptors que se usaron
        close(fd_read);
        close(fd_write);

        // Se ejecuta el comando
        scommand_exec(pipeline_front(apipe));
    }
    else if (pid_first > 0)
    {
        // Elimina un comando del pipe y aumenta el contador de procesos hijo
        pipeline_pop_front(apipe);
        active_child_processes++;
    }

    // Se generan el fork para el segundo comando
    pid_t pid_second = fork();
    if (pid_second < 0)
    {
        print_execute_error("Error con el fork");
    }
    if (pid_second == 0)
    {

        int res_dup = dup2(fd_read, STDIN_FILENO);
        if (res_dup < 0)
        {
            print_execute_error("Error con el dup");
            _exit(EXIT_FAILURE);
        }

        // Se cierran todos los file descriptors que se usaron
        close(fd_read);
        close(fd_write);

        // Se ejecuta el comando
        scommand_exec(pipeline_front(apipe));
    }
    else if (pid_second > 0)
    {
        // Elimina un comando del pipe y aumenta el contador de procesos hijo
        pipeline_pop_front(apipe);
        active_child_processes++;
    }

    // Se cierran todos los file descriptors que se usaron por completo
    close(fd_read);
    close(fd_write);

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

        // Se espera a que todos los hijos terminen uno por uno
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
            print_execute_error("Error con el fork");
        }
        else if (pid == 0)
        {
            // Se conecta el stdin del hijo a un archivo vacio
            /* Como archivo vacio se usa una punta de lectura de pipe
               con punta de escritura cerrada */
            int fd[2];
            int res_pipe = pipe(fd);
            if (res_pipe < 0)
            {
                print_execute_error("Fallo el dup");
                exit(EXIT_FAILURE);
            }

            // Se obtienen los file descriptors para mejor legibilidad
            int fd_read = fd[0];
            int fd_write = fd[1];

            close(fd_write);

            int res_dup2 = dup2(fd_read, STDIN_FILENO);
            if (res_dup2 < 0)
            {
                print_execute_error("Fallo el dup");
                exit(EXIT_FAILURE);
            }

            // Ejecuta todos los comandos del pipeline
            select_mode_pipline(apipe);

            // Y termina para que los hijos pasen a ser hijos del sistema
            exit(EXIT_SUCCESS);
        }
        else
        {
            // Espera a que el hijo termine
            wait_();
        }
    }
}

/*
FALTAN:
 - Implementar el manejo de in/out  de archivos (No esta hecho nada)
*/