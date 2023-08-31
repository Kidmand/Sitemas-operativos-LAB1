#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <string.h>

#include "parsing.h"
#include "parser.h"
#include "command.h"
#include "strextra.h"

/* Imprime por pantalla el error que se mande por "message"
 */
static void print_pipeline_error(char *message)
{
    printf("Se encontro un error en el parsing: %s\n", message);
}

/* Termina de consumir todo el parser. (Incluso el \n)
 * Retorna true si encontro basura y false en caso contrario.
 */
static bool parse_all_rest(Parser p)
{
    bool garbage, res = false;
    parser_garbage(p, &garbage);
    char *garbage_str = parser_last_garbage(p);
    if (garbage)
    {
        res = true;
        print_pipeline_error(garbage_str);
    }
    free(garbage_str);

    return res;
}

/* Crea el comando simple a partir del parser.
 * Si encuentra un error de sintaxis retorna NULL.
 */
static scommand parse_scommand(Parser p)
{
    scommand cmd = scommand_new();
    arg_kind_t arg_type;
    char *arg = NULL;
    bool finish_cmd = false;

    while (!finish_cmd)
    {

        arg = parser_next_argument(p, &arg_type);
        if (arg == NULL)
        {
            finish_cmd = true;
            free(arg);
        }
        else
        {
            switch (arg_type)
            {
            case ARG_NORMAL:
                scommand_push_back(cmd, arg);
                break;
            case ARG_INPUT:
                scommand_set_redir_in(cmd, arg);
                break;
            case ARG_OUTPUT:
                scommand_set_redir_out(cmd, arg);
                break;
            }
        }
    }

    // Manejo de errores

    // No se paso ningun argumeto al comando.
    if (scommand_is_empty(cmd))
    {
        cmd = scommand_destroy(cmd);
        cmd = NULL;
        print_pipeline_error("No se pudo leer ningun argumento");
    }

    return cmd;
}

pipeline parse_pipeline(Parser p)
{
    assert(p != NULL && !parser_at_eof(p));
    pipeline result = pipeline_new();
    scommand cmd = NULL;
    bool error = false, another_pipe = true;

    while (another_pipe && !error)
    {
        parser_skip_blanks(p);
        cmd = parse_scommand(p);
        error = (cmd == NULL); /* Comando inválido al empezar */
        if (!error)
        {
            parser_op_pipe(p, &another_pipe);
            pipeline_push_back(result, cmd);
        }
    }

    // Seteo si se ejecuta en background o foreground
    bool was_op_background;
    parser_op_background(p, &was_op_background);
    pipeline_set_wait(result, !was_op_background);

    /* Termina de leer lo restante y revisa si hay basura.
     * Consumir todo lo que hay inclusive el \n
     * Tolerancia a espacios posteriores.
     */
    bool garbage = parse_all_rest(p);

    // Manejo de errores
    if (error || garbage)
    {
        /* Si hubo error, hacemos cleanup */
        result = pipeline_destroy(result);
        result = NULL;
        return result;
    }

    /*  */

    return result;
}
