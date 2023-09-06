#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#include "execute.h"
#include "command.h"
#include "builtin.h"
#include "tests/syscall_mock.h"

#define READ_END 0  /* index pipe extremo escritura */
#define WRITE_END 1 /* index pipe extremo lectura */
#define NUM_PROCESS 10

/* Función para los mensajes de error */
static void print_execute_error(char *message)
{
    printf("ERROR en el execute: %s\n", message);
}

/* Función para los mensajes de error del wait */
static void wait_(int pc_id)
{
    pid_t wait_response_for_background = wait(&pc_id);

    if (wait_response_for_background == -1)
        print_execute_error("Ocurrio un error al ejecutar el wait()\n");
}

/* Funcion encargada de ejecutar programas externos*/
static void scommand_external_exec(scommand cmd)
{
    assert(cmd != NULL);

    // Obtengo los argumentos
    char **argv = scommand_to_argv(cmd);

    char *input_filename = scommand_get_redir_in(cmd);
    char *output_filename = scommand_get_redir_out(cmd);

    // Configurar redirecciones de entrada y salida(si existen)
    if (input_filename)
    {
        int input_fd = open(input_filename, O_RDONLY, S_IRUSR | S_IRGRP); // lee el archivo input.
        if (input_fd == -1)
        { // detecta si hubo un error al leer el archivo.
            perror("Error al abrir el archivo de entrada");
            exit(EXIT_FAILURE);
        }
        dup2(input_fd, STDIN_FILENO); // redirección de la entrada estándar
        close(input_fd);
    }

    if (output_filename)
    {
        int output_fd = open(output_filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH); // abre,crea o "reemplaza" el archivo.
        if (output_fd == -1)
        { // detecta si hubo un error en el paso anterior.
            perror("Error al abrir el archivo de salida");
            exit(EXIT_FAILURE);
        }
        dup2(output_fd, STDOUT_FILENO); // redirige la salida estándar del programa al archivo.
        close(output_fd);
    }

    execvp(argv[0], argv); // Ejecuta el proceso.

    // Si sigue ejecutando es porque fallo
    print_execute_error("Fallo la ejecucion del proceso.");

    exit(EXIT_FAILURE);
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

        if (conection == 0) // Es el hijo
        {
            scommand_exec(cmd);
        }
        else if (conection > 0) // Es el padre
        {
            wait_(conection);
        }
        else
        {
            print_execute_error("Error al ejecutar el fork\n");
        }
    }
}

/* Funcion encargada de ejecutar un pipeline de dos elementos*/
static void execute_command_multipe(pipeline apipe)
{
    assert(apipe != NULL && pipeline_length(apipe) >= 2u);
    int length = pipeline_length(apipe);

    /* ---- INICIALIZA EL PIPE ----*/

    // Se crean los pipes
    pid_t pipes_fd[NUM_PROCESS][2];

    for (int i=0; i<length; i++) {
        if(pipe(pipes_fd[i]) == -1) {
            printf("Error al crear el pipe\n");
        }
    }
    
    pid_t pid_ids[NUM_PROCESS];
    int j=1; 
    int p=0;

    for (int i=0; i<length; i++) {

        pid_ids[i] = fork();

        if (pid_ids[i] == 0) {
            if (i > 0) {
                // Redirección del stdin desde el extremo de lectura del pipe anterior
                close(pipes_fd[j][1]);
                dup2(pipes_fd[j][0], STDIN_FILENO);
                close(pipes_fd[j][0]);
                j+=2;
                scommand_exec(pipeline_front(apipe));
            }

            if (i < length-1 ) {
                // Redirección del stdout al extremo de escritura del pipe actual
                
                close(pipes_fd[p][0]);
                dup2(pipes_fd[p][1], STDOUT_FILENO);
                close(pipes_fd[p][1]);
                p+=2;
                printf("Esto se ejecuta?");
                scommand_exec(pipeline_front(apipe));
            }

        } else if (pid_ids[i] > 0) { 
         pipeline_pop_front(apipe);
    }

    for (int i = 0; i < length - 1; i++) {
        close(pipes_fd[i][0]);
        close(pipes_fd[i][1]);
    }

    // Esperar a que los procesos hijos terminen
    for (int i = 0; i < length; i++) {
        wait(&pid_ids[i]);
      }

    }
   /* // Se abre el pipe
    int res_pipe = pipe(fd);
    if (res_pipe < 0)
    {
        print_execute_error("Fallo el pipe");
        // se cierran los pipes que ya se abrieron para no generar bugs
        close(fd[0]);
        close(fd[1]);
    }

    // ---- FINALIZA LA INICIALIZCION DEL PIPE ----

    // Se generan el fork para el primer comando
    pid_t pid_first = fork();

    if (pid_first < 0)
    {
        print_execute_error("Error con el fork");
    }
    if (pid_first == 0)
    {
        // Como no es el ultimo comando
        close(fd[READ_END]); // cierra el extremo de lectura del pipe (tuberia), se lo conoce como read

        int res_dup = dup2(fd[WRITE_END], STDOUT_FILENO);
        if (res_dup < 0)
        {
            print_execute_error("Error con el dup");
            _exit(EXIT_FAILURE);
        }

        // Se cierran todos los file descriptors que se usaron
        close(fd[WRITE_END]);

        // Se ejecuta el comando
        scommand_exec(pipeline_front(apipe));
    }
    else if (pid_first > 0)
    {
        // El padre espera al primer comaando ejecutado
        pid_t wait_pid_first = wait(&pid_first);
        if (wait_pid_first == -1)
        {
            print_execute_error("Ocurrio un error al ejecutar el wait() del primero comando\n");
        }

        close(fd[WRITE_END]);

        // Elimina un comando del pipe y aumenta el contador de procesos hijo
        pipeline_pop_front(apipe);

        // Se generan el fork para el segundo comando
        pid_t pid_second = fork();

        if (pid_second < 0)
        {
            print_execute_error("Error con el fork");
        }
        if (pid_second == 0)
        {

            int res_dup = dup2(fd[READ_END], STDIN_FILENO);
            if (res_dup < 0)
            {
                print_execute_error("Error con el dup");
                _exit(EXIT_FAILURE);
            }

            // Se cierran todos los file descriptors que se usaron
            close(fd[READ_END]);

            // Se ejecuta el comando
            scommand_exec(pipeline_front(apipe));
        }
        else if (pid_second > 0)
        {
            // El padre espera al segundo comando ejecutado
            pid_t wait_pid_second = wait(&pid_second);
            if (wait_pid_second == -1)
                print_execute_error("Ocurrio un error al ejecutar el wait() del segundo comando\n");

            // Elimina un comando del pipe y aumenta el contador de procesos hijo
            pipeline_pop_front(apipe);
            // Se cierran todos los file descriptors que se usaron por completo
            close(fd[READ_END]);
            close(fd[WRITE_END]);
        }
    }*/
}

/* Funcion encargada de selecionar el modo de ejecucion de un pipeline*/
static void select_mode_pipline(pipeline apipe)
{
    if (pipeline_length(apipe) == 1)
    {
        execute_command_single(apipe); // Ejecuta el comando, ya sea interno o externo si no existe un pipe (|)
    }
    else if (pipeline_length(apipe) >=2)
    {
        execute_command_multipe(apipe);
    }
    else
    {
        print_execute_error("La terminal solo toma 2 comandos como maximo");
    }
}

/* Funcion encargada de ejecutar un pipeline en su totalidad*/
void execute_pipeline(pipeline apipe)
{
    assert(apipe != NULL);
    if (pipeline_get_wait(apipe))
    {
        select_mode_pipline(apipe);
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
    }
}
