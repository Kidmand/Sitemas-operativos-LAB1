#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#include "command.h"
#include "builtin.h"

static const char *internal_commands[] = {"cd", "help", "exit", "pwd"};

/* Imprime por pantalla el error que se mande por "message"
 */
static void print_internal_cmf_error(char *message)
{
    printf("ERROR con los comandos internos: %s\n", message);
}

// Funciones auxiliares de comandos internos:
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
static void f_cd(scommand args)
{
    if (scommand_length(args) > 1)
    {
        print_internal_cmf_error("El comando 'cd' unicamente recibe un argumento.");
    }
    else
    {
        char *directory = scommand_front(args);
        int result = chdir(directory);
        if (result < 0)
        {
            print_internal_cmf_error("Error al cambiar el directorio de trabajo");
        }
    }
}

static void f_help(scommand args)
{
    if (scommand_length(args) > 1)
    {
        print_internal_cmf_error("El comando 'help' no recibe argumentos.");
    }
    else
    {
        printf("\nShell   : MyBash\n");
        printf("Autores : Ramiro, Matias, Mora, Daian.\n\n");
        printf("Comandos Internos:\n");
        printf("  - cd   : Recibe una ruta donde moverse desde el directorio actual.\n");
        printf("  - pwd  : Imprime el directorio de trabajo actual. \n");
        printf("  - help : Proporciona informacion al usuario sobre los comandos disponibles.\n");
        printf("  - exit : La terminal finaliza de forma correcta.\n\n");
    }
}
// Debe mostrar un mensaje por la salida estándar indicando el nombre del shell, el nombre de sus autores
// y listar los comandos internos implementados con una breve descripción de lo que hace cada uno.

static void f_exit(scommand args)
{
    if (scommand_length(args) > 1)
    {
        print_internal_cmf_error("El comando 'exit' no recibe argumentos.");
    }
    else
    {
        exit_mybash = true;
    }
}

static void f_pwd(scommand args)
{
    if (scommand_length(args) > 1)
    {
        print_internal_cmf_error("El comando 'pwd' no recibe argumentos.");
    }
    else
    {
        char currentDir[1024];
        getcwd(currentDir, sizeof(currentDir));
        printf("%s\n", currentDir);
    }
}

// Funciones del TAD:
bool builtin_is_internal(scommand cmd)
{

    assert(!scommand_is_empty(cmd));

    char *comando = scommand_front(cmd);

    return is_internal(comando);
}

bool builtin_alone(pipeline p)
{

    assert(!pipeline_is_empty(p));

    return pipeline_length(p) == 1 && builtin_is_internal(pipeline_front(p));
}

void builtin_run(scommand cmd)
{

    assert(builtin_is_internal(cmd));

    char *comando = scommand_front(cmd);

    if (strcmp(comando, internal_commands[0]) == 0)
    {
        scommand_pop_front(cmd); // cmd solo contiene los argumentos ahora
        f_cd(cmd);
    }

    else if (strcmp(comando, internal_commands[1]) == 0)
    {
        f_help(cmd);
    }
    else if (strcmp(comando, internal_commands[2]) == 0)
    {
        f_exit(cmd);
    }
    else if (strcmp(comando, internal_commands[3]) == 0)
    {
        f_pwd(cmd);
    }
}