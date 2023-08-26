#include <stdio.h>
#include <unistd.h>

int main()
{
    int pipefd[2];
    /*
    pipefd: Un arreglo de dos enteros que servirá para almacenar los descriptores de archivo del conducto. 
            pipefd[0] se utiliza para la lectura y pipefd[1] para la escritura.
    */
    char buffer[20];
    pipe(pipefd);

    if (fork() == 0)
    {
        close(pipefd[0]); // Cerramos el extremo de lectura
        write(pipefd[1], "¡Hola, mundo!", 13);
        close(pipefd[1]);
    }
    else
    {
        close(pipefd[1]); // Cerramos el extremo de escritura
        read(pipefd[0], buffer, 13);
        printf("Mensaje recibido: %s\n", buffer);
        close(pipefd[0]);
    }

    return 0;
}
