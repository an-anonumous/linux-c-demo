#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>

int main()
{
    pid_t pid =    fork();
    if(pid == -1)
    {
        perror("fork");
        exit(-1);
    }
    else if(pid == 0 )
    {

        int i = 0;
        while(1)
        {
            printf("%d\n", ++i);
        }
    }
    else
    {
        sleep(1);
        int res =        kill(pid, SIGKILL);
        if(res == -1)
        {
            perror("kill");
        }
    }

    printf("child process is killed\n");


}
