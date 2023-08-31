#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../parsing.h"

int main(int argc, char *argv[])
{
    printf("Testeando el modulo parsing.\n");

    pipeline pipe;
    Parser input;

    input = parser_new(stdin);
    pipe = parse_pipeline(input);

    if (pipe != NULL)
    {
        char *string_pipline = pipeline_to_string(pipe);
        printf("El pipline completo es :\n%s\n", string_pipline);
        free(string_pipline);
        pipe = pipeline_destroy(pipe);
    }

    input = parser_destroy(input);

    return EXIT_SUCCESS;
}