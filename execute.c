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

static void comand_internal_external(pipeline apipe, scommand cmd)
{

    if (builtin_is_internal(cmd))
    {
        builtin_run(cmd);
    }
    else
    {
        external_run(cmd);
    }
}

static void execute_process_son(pipeline apipe, scommand cmd, int fd[2])
{

    cmd = pipeline_front(apipe); // Saca el primer comando de apipe

    close(fd[0]);               // cierra el extremo de lectura del pipe (tuberia), se lo conoce como read
    dup2(fd[1], STDOUT_FILENO); // redirige la salida standar (stdout) al extremo de escritura del pipe (tuberia).
    close(fd[1]);               // cierra el extremo de lectura del pipe (tuberia), se lo conoce como write

    comand_internal_external(apipe, cmd); // Ejecuta el comando, ya sea interno o externo
}

static void execute_process_father(pipeline apipe, scommand cmd, int fd[2])
{

    pid_t wait_response = wait(NULL);

    if (wait_response == -1)
    {
        print_execute_error("Ocurrio un error al ejecutar el wait()\n");
    }

    pipeline_pop_front(apipe); /* avanza al siguiente comando, en donde la entrada stdin de este comando
                                       estará en el extremo de lectura del pipe, es decir fd[0] */
    cmd = pipeline_front(apipe);

    close(fd[1]);              // cierra el extremo de escritura del pipe (tuberia), ya que estamos con proceso padre
    dup2(fd[0], STDIN_FILENO); // redirige la entrada standar (stdin) al extremo de lectura del pipe (tuberia).
    close(fd[0]);              // cierra el extremo de lectura del pipe (tuberia)

    comand_internal_external(apipe, cmd); // Ejecuta el comando, ya sea interno o externo
}

static void wait_(pipeline apipe)
{
    pid_t wait_response_for_background = wait(NULL);
    if (wait_response_for_background == -1)
        print_execute_error("Ocurrio un error al ejecutar el wait()\n");
}

static void execute_all_pipline(pipeline apipe)
{
    scommand cmd;
    int fd[2];

    if (pipe(fd) == -1)
    {
        print_execute_error("Error al declarar la tuberiria pipe: \n");
        exit(EXIT_SUCCESS);
    }

    cmd = pipeline_front(apipe);

    if (pipeline_length(apipe) > 1)
    { // verifica que haya más de 1 comado para que tenga sentido ejecutar un pipe

        pid_t conection_pipe = fork();

        if (conection_pipe == 0)
        {
            execute_process_son(apipe, cmd, fd);
        }
        else if (conection_pipe > 0)
        {
            execute_process_father(apipe, cmd, fd);
        }
        else
        {
            print_execute_error("Error al ejecutar el segundo fork\n");
        }
    }
    else
    {
        comand_internal_external(apipe, cmd); // Ejecuta el comando, ya sea interno o externo si no existe un pipe (|)
    }
}

void execute_pipeline(pipeline apipe)
{
    assert(apipe != NULL);
    if (pipeline_get_wait(apipe))
    {
        execute_all_pipline(apipe);
    }
    else
    {
        pid_t pc_id_for_background = fork(); // Creamos un nuevo proceso para ejecutar todos los comandos.

        if (pc_id_for_background == 0) // El hijo ejecuta la totalidad de los comandos
        {
            execute_all_pipline(apipe);
        }
        else if (pc_id_for_background > 0)
        { // El padre es el encargado de decidir si espera o no al hijo
            wait_(apipe);
        }
        else
        { // Manejo de errores
            print_execute_error("Ocurrio un error al ejecutar el primer fork()\n");
        }
    }
}

/*
FALTAN MUCHAS COSAS:
 - Implementar la funcionalidad del pipe (Se tiene  que pasar el stdout al stdin del siguinte comando) (HECHO)
 - Implementar el manejo de in/out  de archivos (No esta hecho nada)
 - Arreglar CTL+D (Nose porque no anda despues de ejecutar cosas)
*/