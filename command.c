#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <glib.h> //Documentacion de LISTAS = https://docs.gtk.org/glib/struct.List.html
#include <string.h>

#include "command.h"
#include "strextra.h"

/* ------- Funciones auxiliares ------*/

/*
static void show_list(GList *list)
{
    assert(list != NULL);
    GList *current = list;
    while (current != NULL)
    {
        printf("%s\n", (char *)current->data);
        current = g_list_next(current);
    }
}
*/

/* ----------  COMANDO SIMPLE ---------- */

/* Estructura de un comando simple.
 * Es una 3-upla del tipo ([char*], char* , char*).
 */
struct scommand_s
{
    GList *args;
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

    assert(result != NULL && scommand_is_empty(result) /*&&
           scommand_get_redir_in(result) == NULL &&
           scommand_get_redir_out(result) == NULL */
    );

    return result;
}

scommand scommand_destroy(scommand self)
{
    assert(self != NULL);
    g_list_free_full(self->args, free);
    free(self->redir_in);
    free(self->redir_out);

    self->args = NULL;
    self->redir_in = NULL;
    self->redir_out = NULL;

    free(self);
    self = NULL;

    assert(self == NULL);
    return self;
}

/* Modificadores */

void scommand_push_back(scommand self, char *argument)
{
    assert(self != NULL && argument != NULL);

    self->args = g_list_append(self->args, argument);

    assert(!scommand_is_empty(self));
}

void scommand_pop_front(scommand self)
{
    assert(self != NULL && !scommand_is_empty(self));

    gpointer first_element = g_list_nth_data(self->args, 0);

    self->args = g_list_remove(self->args, first_element);

    free(first_element);
    first_element = NULL;
}

void scommand_set_redir_in(scommand self, char *filename)
{
    assert(self != NULL);
    self->redir_in = filename;
}

void scommand_set_redir_out(scommand self, char *filename)
{
    assert(self != NULL);
    self->redir_out = filename;
}

/* Proyectores */

bool scommand_is_empty(const scommand self)
{
    assert(self != NULL);
    return self->args == NULL;
}

unsigned int scommand_length(const scommand self)
{
    assert(self != NULL);
    unsigned int length = 0;

    if (self->args != NULL)
        length = g_list_length(self->args);

    assert((length == 0) == scommand_is_empty(self));
    return length;
}

char *scommand_front(const scommand self)
{
    assert(self != NULL && !scommand_is_empty(self));

    char *res = g_list_nth_data(self->args, 0);

    assert(res != NULL);
    return res;
}

char *scommand_get_redir_in(const scommand self)
{
    assert(self != NULL);
    return self->redir_in;
}

char *scommand_get_redir_out(const scommand self)
{
    assert(self != NULL);
    return self->redir_out;
}

/*ESTO TIENE MEMORY LEEKS, PORQUE strmerge GENERA NUEVA MEMORIA*/
char *scommand_to_string(const scommand self)
{
    assert(self != NULL);
    GList *aux_list = self->args;
    char *result = strdup(""), *aux;
    while (aux_list != NULL)
    {
        result = strmerge(result, g_list_nth_data(aux_list, 0));
        result = strmerge(result, " ");
        aux_list = g_list_next(aux_list);
    }

    if (self->redir_out != NULL)
    {
        result = strmerge(result, " > ");
        result = strmerge(result, self->redir_out);
    }

    if (self->redir_in != NULL)
    {
        result = strmerge(result, " < ");
        result = strmerge(result, self->redir_in);
    }

    assert(scommand_is_empty(self) ||
           scommand_get_redir_in(self) == NULL ||
           scommand_get_redir_out(self) == NULL ||
           strlen(result) > 0);

    return result;
}

/* ---------- COMANDO PIPELINE ---------- */

/* Estructura de un comando con pipeline.
 * Es un 2-upla del tipo ([scommand], bool)
 */
/*
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
*/
/* Modificadores */
/*
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
*/
/* Proyectores */
/*
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
*/