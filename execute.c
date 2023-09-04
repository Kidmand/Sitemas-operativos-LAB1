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
static void external_run(scommand cmd)
{
    assert(cmd != NULL);

    // Obtengo los argumentos
    char **argv = scommand_to_argv(cmd);

    execvp(argv[0], argv); // Ejecuta el proceso.

    // Si sigue ejecutando es porque fallo
    print_execute_error("Fallo la ejecucion del proceso.");
    exit(EXIT_SUCCESS);
}

static void execute_command_single(pipeline apipe)
{
    scommand cmd = pipeline_front(apipe);
    if (builtin_is_internal(cmd))
    {
        builtin_run(cmd);
    }
    else
    {
        pid_t conection = fork();

        if (conection == 0)
        {
            external_run(cmd);
        }
        else if (conection > 0)
        {
            wait_();
        }
        else
        {
            print_execute_error("Error al ejecutar el fork\n");
        }
    }
}


static void execute_command_multipe(pipeline apipe)
{
    int fd[2];

    if (pipe(fd) == -1)
    {
        print_execute_error("Error al declarar la tuberiria pipe: \n");
        exit(EXIT_SUCCESS);
    }

    int fd_read = fd[0];
    int fd_write = fd[1];

    scommand firts_cmd = pipeline_front(apipe);

    pid_t conection_pipe1 = fork();

    if (conection_pipe1 == 0)
    {
        close(fd_read);                // cierra el extremo de lectura del pipe (tuberia), se lo conoce como read
        dup2(fd_write, STDOUT_FILENO); // redirige la salida standar (stdout) al extremo de escritura del pipe (tuberia).
                    // cierra el extremo de lectura del pipe (tuberia), se lo conoce como write
        if (builtin_is_internal(firts_cmd))
        {
            builtin_run(firts_cmd);

            exit(EXIT_SUCCESS);
        }
        else
        {
            external_run(firts_cmd);
        }
    }
     pid_t conection_pipe2 = fork();
     if (conection_pipe2 == 0)
    {
        pipeline_pop_front(apipe); /* avanza al siguiente comando, en donde la entrada stdin de este comando
                                       estará en el extremo de lectura del pipe, es decir fd[0] */
                                       
        close(fd_write);             // cierra el extremo de escritura del pipe (tuberia), ya que estamos con proceso padre
        dup2(fd_read, STDIN_FILENO); // redirige la entrada standar (stdin) al extremo de lectura del pipe (tuberia).   
    
        execute_command_single(apipe);  
    }
    close (fd[0]);
    close (fd[1]);

    wait(NULL);
    wait(NULL);
}

static void select_mode_pipline(pipeline apipe)
{
    if (pipeline_length(apipe) == 1)
    {
        execute_command_single(apipe); // Ejecuta el comando, ya sea interno o externo si no existe un pipe (|)
    }
    else if (pipeline_length(apipe) == 2)
    {
        execute_command_multipe(apipe);
    }
    else
    {
        print_execute_error("La terminal solo toma 2 comandos como maximo \n");
    }
}

void execute_pipeline(pipeline apipe)
{
    assert(apipe != NULL);

    if (pipeline_get_wait(apipe))
    {
        select_mode_pipline(apipe);
    
    }
    else
    {
        print_execute_error("Todavia no esta implementado el background \n");
    }
}

/*
FALTAN MUCHAS COSAS:
 - Implementar la funcionalidad del pipe (Se tiene  que pasar el stdout al stdin del siguinte comando) (HECHO)
 - Implementar el manejo de in/out  de archivos (No esta hecho nada)
 - Arreglar CTL+D (Nose porque no anda despues de ejecutar cosas)
*/