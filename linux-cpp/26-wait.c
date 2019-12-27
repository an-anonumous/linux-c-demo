#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

int main()
{
    printf("i am parent process ,my pid =%d \n", getpid());

    //pid_t fork(void);
    pid_t pid =    fork();
    if(pid == -1)
    {
        perror("fork");
    }
    else if(pid  == 0)
    {
        // child progress.
        printf("i am child ,my pid = %d my parent progress id is %d.\n", getpid(), getppid());
//      int a = 100 / 0;   // single 8;

        char cmd[256] = {0};
        sprintf(cmd, "kill -9 %d \n ", getpid());  // 9
        system(cmd);
        return -1;
    }
    else
    {
        //parent procress.

        sleep(1);

        int  wstatus;
        // pid_t wait(int *wstatus);
        pid_t pid2 = wait(&wstatus);
        if(pid2 == -1)
        {
            perror("wait");
            exit(-1);
        }
        else
        {
            if(WIFEXITED(wstatus))
            {
                // exited nomally
                printf("child progress exited with value %d \n", WEXITSTATUS(wstatus));
            }

            if(WIFSIGNALED(wstatus))
            {
                //child process was killd by single.
                printf("child progress killed by single %d \n", WTERMSIG(wstatus));
            }
        }


    }

}
