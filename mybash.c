#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "command.h"
#include "execute.h"
#include "parser.h"
#include "parsing.h"
#include "builtin.h"

static void show_prompt(void)
{
    char mybash[] = "mybash:~";
    char pwd[1024];
    getcwd(pwd, sizeof(pwd));
    strcat(pwd, "$");
    printf("\x1b[36m");
    printf("\x1b[1m");
    printf("%s", "Daian ");
    printf("\x1b[33m");
    printf("\x1b[1m");
    printf("%s", "Mora ");
    printf("\x1b[35m");
    printf("\x1b[1m");
    printf("%s", "Ramiro ");
    printf("\x1b[31m");
    printf("\x1b[1m");
    printf("%s", "Matias\n");
    printf("\x1b[32m");
    printf("\x1b[1m");
    printf("%s", mybash);
    printf("\x1b[34m");
    printf("\x1b[1m");
    printf("%s ", pwd);
    printf("\x1b[0m");

    fflush(stdout);
}

int main(int argc, char *argv[])
{
    pipeline pipe = NULL;
    Parser input = NULL;

    bool exit_mybash = false;

    input = parser_new(stdin);
    while (!exit_mybash)
    {
        // Si se preciono CTL+D => Salimos de la terminal.
        exit_mybash = parser_at_eof(input);

        if (!exit_mybash)
        {
            show_prompt();

            // Se parsea el input.
            pipe = parse_pipeline(input);

            // Si no hubo errores se ejecuta el pipeline
            if (pipe != NULL)
            {
                execute_pipeline(pipe);
                pipe = pipeline_destroy(pipe);
            }
        }
    }
    printf("\n");
    parser_destroy(input);
    input = NULL;
    return EXIT_SUCCESS;
}
