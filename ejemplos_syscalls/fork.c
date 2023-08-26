#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    printf("Este es mi primer fork() \n");

    pid_t fork_response = fork();

    if (fork_response == 0)
    {
        printf("Hola yo soy el HIJO\n");
    }
    else if (fork_response > 0)
    {
        printf("Hola yo soy el PADRE\n");
    }
    else
    {
        printf("Ocurrio un error al ejecutar el fork() \n");
    }

    return 0;
}