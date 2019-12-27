#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    printf("I shall take no wife, hold no lands, father no children.  mypid =%d \n", getpid());

    int i = 0;
    for(; i < 50000; i++)
    {
        pid_t pid = fork();
        if(pid == -1)
        {
            perror("fork error.\n");
            exit(-1);
        }
        else if(pid == 0)
        {
            //child processs.
            break;   // **************************************
        }
        else
        {
//            printf("i am parent progess,mypid is %d \n", getpid());
        }
    }

    if(i < 5)
    {
        //child progress.
        printf("i am child ,mypid = %d, i = %d ,my parent id is %d \n", getpid(), i, getppid());
    }
    else
    {
        printf("i am parent progess,mypid is %d,my parent id =%d \n", getpid(), getppid());
    }

    puts("I am the fire that burns against the cold, the light that brings the dawn, the horn that wakes the sleepers, the shield that guards the realms of men. ");

}
