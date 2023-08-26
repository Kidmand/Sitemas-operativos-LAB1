#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    printf("Este es mi primer fork() que le agrego wait()\n");

    pid_t fork_response = fork();

    if (fork_response == 0)
    {
        printf("Hola yo soy el HIJO, me ejecuto primero\n");
    }
    else if (fork_response > 0)
    {
        pid_t wait_response = wait(NULL);
        if (wait_response == -1)
            printf("Ocurrio un error al ejecutar el wait()\n");

        printf("Hola yo soy el PADRE, me ejecuto ultimo\n");
    }
    else
    {
        printf("Ocurrio un error al ejecutar el fork()\n");
    }

    return 0;
}