#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    int pipefd[2];
    pipe(pipefd);
    // Se crea el pipe, la conecion.

    pid_t pid = fork();

    if (pid == 0)
    {
        // Proceso hijo
        close(pipefd[0]); // Cerramos el extremo de lectura

        // Redirigimos la salida estándar que producira el ls al extremo de escritura del pipe
        dup2(pipefd[1], STDOUT_FILENO);
        
        close(pipefd[1]); // Cerramos el descriptor de archivo duplicado

        // Ejecutamos el comando ls
        execlp("ls", "ls", NULL);
        /*
        La función execlp toma como primer argumento el nombre del programa que deseas ejecutar y los argumentos se pasan como argumentos separados por coma después del nombre del programa. El último argumento debe ser NULL.
        Busca automáticamente el programa en los directorios listados en la variable de entorno PATH, lo que significa que no es necesario proporcionar la ruta completa al archivo ejecutable.
        */
        perror("execlp");
        exit(1);
    }
    else if (pid > 0)
    {
        // Proceso padre
        close(pipefd[1]); // Cerramos el extremo de escritura

        // Esperamos a que el proceso hijo termine
        wait(NULL);

        // Redirigimos la entrada estándar al extremo de lectura del pipe que va a leer wc.
        dup2(pipefd[0], STDIN_FILENO);

        close(pipefd[0]); // Cerramos el descriptor de archivo duplicado

        // Ejecutamos el comando wc -l
        execlp("wc", "wc", "-l", NULL);
        perror("execlp"); // En caso de error
        exit(1);
    }
    else
    {
        perror("fork"); // En caso de error
    }

    return 0;
}
