#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    printf("Vamos a ejecutar un ls del directorio actual: \n\n");
    pid_t fork_response = fork();

    if (fork_response == 0)
    {
        // Arreglo de argumentos para execv
        char *args[] = {"ls", "-l", NULL};
        int exexv_response = execv("/bin/ls", args);

        if (exexv_response == -1)
            printf("Ocurrio un error al ejecutar el execv()\n");
    }
    else if (fork_response > 0)
    {
        pid_t wait_response = wait(NULL);
        if (wait_response == -1)
            printf("Ocurrio un error al ejecutar el wait()\n");
    }
    else
    {
        printf("Ocurrio un error al ejecutar el fork()\n");
    }

    return 0;
}