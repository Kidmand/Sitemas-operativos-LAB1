#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "parsing.h"
#include "parser.h"
#include "command.h"

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
        cmd = parse_scommand(p);
        parser_op_pipe(p, &another_pipe);
        pipeline_push_back(result, cmd);
        error = (cmd == NULL); /* Comando inválido al empezar */
    }

    // Seteo si se ejecuta en background o foreground
    bool was_op_background;
    parser_op_background(p, &was_op_background);
    pipeline_set_wait(result, was_op_background);

    bool garbage;
    parser_garbage(p, &garbage);
    if (garbage)
    {
        char *garbage_str = parser_last_garbage(p);
        printf("Se borro la siguiente basura: '%s'\n", garbage_str);
    }

    // TIENE UN BYTE DE MEMORYLIKE

    /* Tolerancia a espacios posteriores */
    /* Consumir todo lo que hay inclusive el \n */
    /* Si hubo error, hacemos cleanup */

    return result; // MODIFICAR
}
