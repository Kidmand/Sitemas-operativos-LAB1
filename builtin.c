#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "builtin.h"
#include "command.h"
#include "strextra.h"
#include "tests/syscall_mock.h"

// exit -----------------------------------------------------------

static bool builtin_scommand_is_exit(const scommand cmd)
{
    assert(cmd != NULL);

    return strcmp(scommand_front(cmd), "exit") == 0;
}

static void builtin_run_exit(const scommand cmd)
{
    assert(cmd != NULL && builtin_scommand_is_exit(cmd));

    _exit(EXIT_SUCCESS);
}

// cd  -----------------------------------------------------------
static bool builtin_scommand_is_cd(const scommand cmd)
{
    assert(cmd != NULL);
    return strcmp(scommand_front(cmd), "cd") == 0;
    // Si son iguales strcmp devuelve 0
}

static void builtin_run_cd(const scommand cmd)
{
    assert(cmd != NULL && builtin_scommand_is_cd(cmd));

    unsigned int length = scommand_length(cmd);
    if (length <= 2u)
    {

        char *input_path = NULL;
        unsigned int path_length = 0u;
        if (length > 1u)
        {
            scommand_pop_front(cmd);
            input_path = scommand_front(cmd);
            path_length = strlen(input_path);
        }
        int ret_code = 0;
        char *home_path = getenv("HOME");

        /* Si el argumento de chdir comienza con / el path se toma desde equipo
           (ósea como path absoluto) y si empieza con ./ o sin nada se toma desde
           el directorio actual. También, chdir acepta .. para ir un directorio
           para arriba.
        */

        if (path_length > 1u)
        {
            char *relative_path = NULL;
            char *full_path = NULL;
            if (input_path[0] == '~' && input_path[1] == '/' &&
                home_path != NULL)
            {
                /* Caso en el que el ~ es seguido por un /, por ejemplo ~/Documentos, esto
                se considera como un directorio relativo al directorio principal.
                */
                relative_path = &input_path[1];
                full_path = strmerge(home_path, relative_path);

                ret_code = chdir(full_path);
            }
            else if (input_path[0] == '\'' && input_path[1] == '~' &&
                     input_path[2] == '\'')
            {
                relative_path = &input_path[3];
                full_path = strmerge("~", relative_path);
                ret_code = chdir(full_path);
                if (ret_code != 0)
                {
                    ret_code = chdir(input_path);
                }
            }
            else
            {
                /* Maneja cualquier caso en el que no se encuentre un ~ o '~' antes de /
                 */
                ret_code = chdir(input_path);
            }
            free(full_path);
            full_path = NULL;
        }
        else
        {
            if (input_path == NULL || input_path[0] == '~' ||
                strcmp(input_path, "") == 0)
            {
                if (home_path == NULL)
                {
                    ret_code = chdir("~");
                }
                else
                {
                    ret_code = chdir(home_path);
                }
            }
            else
            {
                ret_code = chdir(input_path);
            }
        }
        if (ret_code != 0)
        {
            /* La función chdir deja un mensaje en algún lado, con perror se puede
               imprimir el último mensaje, por lo cuál, en caso de error se la usa.
               perror toma un string, e imprime primero ese string, y después el
               mensaje de error. man perror para mas información
            */
            perror("mybash: cd");
        }
    }
    else
    {
        printf("mybash: cd: demasiados argumentos\n");
    }
}

// help -----------------------------------------------------------
static bool builtin_scommand_is_help(const scommand cmd)
{
    assert(cmd != NULL);
    return strcmp(scommand_front(cmd), "help") == 0;
    // Si son iguales strcmp devuelve 0
}

static void builtin_run_help(scommand args)
{
    if (scommand_length(args) > 1)
    {
        perror("El comando 'help' no recibe argumentos.");
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

// Chequeo

bool builtin_is_internal(const scommand cmd)
{
    assert(cmd != NULL);
    return builtin_scommand_is_exit(cmd) || builtin_scommand_is_cd(cmd) || builtin_scommand_is_help(cmd);
}

// Ejecución

void builtin_run(const scommand cmd)
{
    assert(cmd != NULL && builtin_is_internal(cmd));
    if (builtin_scommand_is_cd(cmd))
    {
        builtin_run_cd(cmd);
    }
    else if (builtin_scommand_is_help(cmd))
    {
        builtin_run_help(cmd);
    }
    else
    {
        builtin_run_exit(cmd);
    }
}
