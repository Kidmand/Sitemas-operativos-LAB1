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

    assert(result != NULL && scommand_is_empty(result) &&
           scommand_get_redir_in(result) == NULL &&
           scommand_get_redir_out(result) == NULL);

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

char *scommand_to_string(const scommand self)
{
    assert(self != NULL);
    char *result = strdup("");
    GList *aux_list = self->args;

    while (aux_list != NULL)
    {
        result = strconcat(result, g_list_nth_data(aux_list, 0));
        result = strconcat(result, " ");
        aux_list = g_list_next(aux_list);
    }

    if (self->redir_out != NULL)
    {
        result = strconcat(result, " > ");
        result = strconcat(result, self->redir_out);
    }

    if (self->redir_in != NULL)
    {
        result = strconcat(result, " < ");
        result = strconcat(result, self->redir_in);
    }

    assert(scommand_is_empty(self) ||
           scommand_get_redir_in(self) == NULL ||
           scommand_get_redir_out(self) == NULL ||
           strlen(result) > 0);

    return result;
}

char **scommand_to_argv(scommand self)
{
    assert(self != NULL);

    unsigned int n = scommand_length(self);
    char **argv = calloc(sizeof(char *), n + 1);

    if (argv != NULL)
    {
        for (unsigned int j = 0; j < n; j++)
        {
            char *arg = scommand_front(self);
            scommand_pop_front(self);
            argv[j] = arg;

            assert(argv[j] != NULL);
        }
        argv[n] = NULL;
    }

    assert(self != NULL &&
           ((argv == NULL) !=
            (scommand_is_empty(self) && argv != NULL && argv[n] == NULL)));
    return (argv);
}
/* ---------- COMANDO PIPELINE ---------- */

/* Estructura de un comando con pipeline.
 * Es un 2-upla del tipo ([scommand], bool)
 */

struct pipeline_s
{
    GList *cmds;
    bool wait;
};

pipeline pipeline_new(void)
{
    pipeline result = malloc(sizeof(struct pipeline_s));
    if (result == NULL)
    {
        perror("Error al alojar memoria (malloc)");
        exit(EXIT_FAILURE);
    }
    result->cmds = NULL;
    result->wait = true;

    assert(result != NULL && pipeline_is_empty(result) &&
           pipeline_get_wait(result));

    return result;
}

static void void_scommand_destroy(void *self)
{
    scommand killme = self;
    scommand_destroy(killme);
}

pipeline pipeline_destroy(pipeline self)
{
    assert(self != NULL);
    g_list_free_full(self->cmds, void_scommand_destroy);
    self->cmds = NULL;

    free(self);
    self = NULL;

    assert(self == NULL);
    return self;
}

/* Modificadores */

void pipeline_push_back(pipeline self, scommand sc)
{
    assert(self != NULL && sc != NULL);

    self->cmds = g_list_append(self->cmds, sc);

    assert(!pipeline_is_empty(self));
}

void pipeline_pop_front(pipeline self)
{
    assert(self != NULL && !pipeline_is_empty(self));

    gpointer first_element = g_list_nth_data(self->cmds, 0);

    self->cmds = g_list_remove(self->cmds, first_element);

    scommand_destroy(first_element);
    first_element = NULL;
}

void pipeline_set_wait(pipeline self, const bool w)
{
    assert(self != NULL);
    self->wait = w;
}

/* Proyectores */

bool pipeline_is_empty(const pipeline self)
{
    assert(self != NULL);
    return self->cmds == NULL;
}

unsigned int pipeline_length(const pipeline self)
{
    assert(self != NULL);
    unsigned int length = 0;

    if (self->cmds != NULL)
        length = g_list_length(self->cmds);

    assert((length == 0) == pipeline_is_empty(self));
    return length;
}

scommand pipeline_front(const pipeline self)
{
    assert(self != NULL && !pipeline_is_empty(self));

    scommand res = g_list_nth_data(self->cmds, 0);

    assert(res != NULL);
    return res;
}

bool pipeline_get_wait(const pipeline self)
{
    assert(self != NULL);
    return self->wait;
}

char *pipeline_to_string(const pipeline self)
{
    assert(self != NULL);
    char *result = strdup(""), *aux = NULL;
    GList *aux_list = self->cmds;
    scommand aux_command = NULL;

    while (aux_list != NULL)
    {
        aux_command = g_list_nth_data(aux_list, 0);
        aux = scommand_to_string(aux_command);
        result = strconcat(result, aux);
        free(aux);
        aux_list = g_list_next(aux_list);
        if (aux_list != NULL)
            result = strconcat(result, "| ");
    }

    if (!self->wait)
        result = strconcat(result, "& ");

    assert(pipeline_is_empty(self) || pipeline_get_wait(self) || strlen(result) > 0);

    return result;
}