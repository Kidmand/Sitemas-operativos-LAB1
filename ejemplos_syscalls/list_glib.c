#include <stdio.h>
#include <glib.h>

int main(int argc, char *argv[])
{
    // Crear una lista enlazada de GLib
    GList *list = NULL;

    // Agregar elementos a la lista
    list = g_list_append(list, "Manzana");
    list = g_list_append(list, "Banana");
    list = g_list_append(list, "Naranja");

    // Recorrer la lista e imprimir los elementos
    GList *current = list;
    while (current != NULL)
    {
        printf("%s\n", (char *)current->data);
        current = g_list_next(current);
    }

    // Liberar la memoria de la lista
    g_list_free(list);

    return 0;
}

// Instalar
// sudo apt-get install libglib2.0-dev

//  Ejecutar
//  gcc list_glib.c `pkg-config --cflags glib-2.0` `pkg-config --libs glib-2.0` -o output && ./output
