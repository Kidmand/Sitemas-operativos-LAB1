#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Esto retrocede una carpeta para atras

int main()
{
    char currentDir[1024];
    if (getcwd(currentDir, sizeof(currentDir)) != NULL) // getcwd() sirve para saber la ruta en la que estamos.
    {
        // Obtenemos el directorio actual
        printf("Directorio actual: %s\n", currentDir);

        // Encontramos la última aparición de '/' en la ruta
        // strrchr() es de <string.h>, hay otras formas de hacer esto.
        char *lastSlash = strrchr(currentDir, '/');
        if (lastSlash != NULL)
        {
            // Truncamos la ruta en el último '/'
            *lastSlash = '\0';
            int chdir_response = chdir(currentDir);
            if (chdir_response == 0)
            {
                // Ya s cambio de directorio, ahora lo compruebo.
                getcwd(currentDir, sizeof(currentDir));
                printf("Directorio padre: %s\n", currentDir);
            }
            else
            {
                perror("Error al cambiar de directorio");
            }
        }
        else
        {
            printf("Ya estás en el directorio raíz\n");
        }
    }
    else
    {
        perror("Error al obtener el directorio actual");
    }

    return 0;
}
