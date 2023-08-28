#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../command.h"

static void comando_simples(void)
{
    printf("Esto es un test de comando simples.\n");
    scommand comand = scommand_new();
    printf("Es vacia : %d - El tamaño es: %u \n", scommand_is_empty(comand), scommand_length(comand));
    scommand_push_back(comand, strdup("ls"));
    printf("El primer elemento es: %s\n", scommand_front(comand));
    printf("Es vacia : %d - El tamaño es: %u \n", scommand_is_empty(comand), scommand_length(comand));

    scommand_pop_front(comand);
    printf("Es vacia : %d - El tamaño es: %u \n", scommand_is_empty(comand), scommand_length(comand));

    scommand_set_redir_in(comand, strdup("./in"));
    scommand_set_redir_out(comand, strdup("./out"));
    printf("La entrada es: %s\n", scommand_get_redir_in(comand));
    printf("La salida es: %s\n", scommand_get_redir_out(comand));

    scommand_push_back(comand, strdup("ls"));
    scommand_push_back(comand, strdup("-l"));
    scommand_push_back(comand, strdup("xd"));
    char *string_comand = scommand_to_string(comand);
    printf("El comando completo es : %s \n", string_comand);

    free(string_comand);

    comand = scommand_destroy(comand);
}

static void test_pipeline(void)
{
    printf("Esto es un test de pipeline.\n");

    pipeline new_p = pipeline_new();
    scommand new_c = scommand_new();
    scommand_push_back(new_c, strdup("ls"));
    scommand_push_back(new_c, strdup("-l"));
    scommand_set_redir_in(new_c, strdup("./in"));
    scommand_set_redir_out(new_c, strdup("./out"));

    scommand new_c2 = scommand_new();
    scommand_push_back(new_c2, strdup("ls"));
    scommand_push_back(new_c2, strdup("-l"));
    scommand_set_redir_in(new_c2, strdup("./in"));
    scommand_set_redir_out(new_c2, strdup("./out"));

    pipeline_push_back(new_p, new_c);
    pipeline_push_back(new_p, new_c2);

    printf("Tiene %d elementos \n", pipeline_length(new_p));
    pipeline_pop_front(new_p);
    printf("Tiene %d elementos \n", pipeline_length(new_p));

    pipeline_set_wait(new_p, false);
    printf("El estado de wait es: %d \n", pipeline_get_wait(new_p));

    scommand get_comand = pipeline_front(new_p);
    char *string_comand = scommand_to_string(get_comand);
    printf("El comando completo es : %s \n", string_comand);
    free(string_comand);

    char *string_pipline = pipeline_to_string(new_p);
    printf("El pipline completo es : %s \n", string_pipline);
    free(string_pipline);

    new_p = pipeline_destroy(new_p);
}
int main(int argc, char *argv[])
{
    comando_simples();

    printf("\n\n");

    test_pipeline();
    return EXIT_SUCCESS;
}
