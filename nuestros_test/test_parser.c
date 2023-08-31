#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../parser.h"

int main(int argc, char *argv[])
{
    printf("Testeando el modulo parser.\n");
    Parser input = parser_new(stdin);

    arg_kind_t arg_type;
    char *palabra = parser_next_argument(input, &arg_type);
    printf("La palabra es: (%s) \n", palabra);
    if (palabra != NULL)
    {
        switch (arg_type)
        {
        case ARG_NORMAL:
            printf("Es normal\n");
            break;
        case ARG_INPUT:
            printf("Es input\n");
            break;
        case ARG_OUTPUT:
            printf("Es output\n");
            break;

        default:
            printf("No se que carajo es\n");
            break;
        }
    }
    else
    {
        printf("Es cualquier cosa\n");
    }
    free(palabra);
    palabra = parser_next_argument(input, &arg_type);
    printf("La palabra es (%s) \n", palabra);
    if (palabra != NULL)
    {
        switch (arg_type)
        {
        case ARG_NORMAL:
            printf("Es normal\n");
            break;
        case ARG_INPUT:
            printf("Es input\n");
            break;
        case ARG_OUTPUT:
            printf("Es output\n");
            break;

        default:
            printf("No se que carajo es\n");
            break;
        }
    }
    else
    {
        printf("Es cualquier cosa\n");
    }
    free(palabra);
    palabra = parser_next_argument(input, &arg_type);
    printf("La palabra es (%s) \n", palabra);

    if (palabra != NULL)
    {
        switch (arg_type)
        {
        case ARG_NORMAL:
            printf("Es normal\n");
            break;
        case ARG_INPUT:
            printf("Es input\n");
            break;
        case ARG_OUTPUT:
            printf("Es output\n");
            break;

        default:
            printf("No se que carajo es\n");
            break;
        }
    }
    else
    {
        printf("Es cualquier cosa\n");
    }
    free(palabra);

    bool was_op_background;
    parser_op_background(input, &was_op_background);
    printf("Tiene un &: %d \n", was_op_background);

    bool was_op_pipe;
    parser_op_pipe(input, &was_op_pipe);
    printf("Tiene un |: %d \n", was_op_pipe);

    parser_skip_blanks(input);

    bool garbage;
    parser_garbage(input, &garbage);
    printf("Se encontraron cosas: %d \n", garbage);
    char *garbage_str = parser_last_garbage(input);
    printf("Lo que se borro fue: %s\n", garbage_str);
    // free(garbage_str);

    // ESTO SE PRENDE SOLO SI TOCAMOS " ctrl-D "
    printf("El parser llego al final: %d \n", parser_at_eof(input));

    input = parser_destroy(input);
    return EXIT_SUCCESS;
}