#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "command.h"
#include "execute.h"
#include "parser.h"
#include "parsing.h"
#include "builtin.h"

static void show_prompt(void)
{
    printf("mybash> ");
    fflush(stdout);
}

int main(int argc, char *argv[])
{
    pipeline pipe;
    Parser input;
    bool quit = false;

    input = parser_new(stdin);
    while (!quit)
    {
        show_prompt();

        // Si se preciono CTL+D => Salimos de la terminal.
        if (parser_at_eof(input))
            exit(0);

        // Se parsea el input.
        pipe = parse_pipeline(input);

        // Si no hubo errores se ejecuta el pipeline
        if (pipe != NULL)
        {
            execute_pipeline(pipe);
            pipe = pipeline_destroy(pipe);
        }
    }
    parser_destroy(input);
    input = NULL;
    return EXIT_SUCCESS;
}
