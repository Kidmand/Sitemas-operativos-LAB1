#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <glib.h> //Documentacion de LISTAS = https://docs.gtk.org/glib/struct.List.html

#include "command.h"

/* ----------  COMANDO SIMPLE ---------- */

/* Estructura de un comando simple.
 * Es una 3-upla del tipo ([char*], char* , char*).
 */
struct scommand_s
{
    GSList *args;
    char *redir_in;
    char *redir_out;
};

scommand scommand_new(void)
{
    scommand result = malloc(sizeof(struct scommand_s));
    if (result == NULL)
    {
        perror("Error al alojar memoria (malloc)");
        exit(EXIT_FAILURE);
    }
    result->args = NULL;
    result->redir_in = NULL;
    result->redir_out = NULL;

    assert(result != NULL && scommand_is_empty(result) &&
           scommand_get_redir_in(result) == NULL &&
           scommand_get_redir_out(result) == NULL);

    return result;
}

scommand scommand_destroy(scommand self)
{
}

/* Modificadores */

void scommand_push_back(scommand self, char *argument)
{
    return 0;
}

void scommand_pop_front(scommand self);
{
    return 0;
}
void scommand_set_redir_in(scommand self, char *filename)
{
    return 0;
}
void scommand_set_redir_out(scommand self, char *filename)
{
    return 0;
}

/* Proyectores */

bool scommand_is_empty(const scommand self)
{
    return 0;
}

unsigned int scommand_length(const scommand self)
{
    return 0;
}

char *scommand_front(const scommand self)
{
    return 0;
}

char *scommand_get_redir_in(const scommand self)
{
    return 0;
}
char *scommand_get_redir_out(const scommand self)
{
    return 0;
}

char *scommand_to_string(const scommand self)
{
    return 0;
}

/* ---------- COMANDO PIPELINE ---------- */

/* Estructura de un comando con pipeline.
 * Es un 2-upla del tipo ([scommand], bool)
 */
struct pipeline_s
{
    GSList *scmds;
    bool wait;
};

pipeline pipeline_new(void)
{
    return 0;
}

pipeline pipeline_destroy(pipeline self)
{
    return 0;
}

/* Modificadores */

void pipeline_push_back(pipeline self, scommand sc)
{
    return 0;
}

void pipeline_pop_front(pipeline self)
{
    return 0;
}

void pipeline_set_wait(pipeline self, const bool w)
{
    return 0;
}

/* Proyectores */

bool pipeline_is_empty(const pipeline self)
{
    return 0;
}

unsigned int pipeline_length(const pipeline self)
{
    return 0;
}

scommand pipeline_front(const pipeline self)
{
    return 0;
}

bool pipeline_get_wait(const pipeline self)
{
    return 0;
}

char *pipeline_to_string(const pipeline self)
{
    return 0;
}