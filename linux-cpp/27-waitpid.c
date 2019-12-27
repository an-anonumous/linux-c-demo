#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#define CHILD_PROCESS_NUM 100

int main()
{
    int pids[CHILD_PROCESS_NUM];
    pid_t pid;
    int i = 0;
    for( ; i < CHILD_PROCESS_NUM; i++)
    {
        pid = fork();
        if(pid == -1)
        {
            perror("fork");
            exit(-1);
        }
        else if(pid == 0)
        {
            //chid progress.
            sleep(20 - i % 10);
            break;
        }
        else
        {
            // parent
            pids[i] = pid;
        }
    }

    if(i == CHILD_PROCESS_NUM)
    {
        // parent

        //pid_t waitpid(pid_t pid, int *wstatus, int options);
        int wstatus;
        pid_t pid = waitpid(pids[1], &wstatus, WNOHANG);
        if(pid == -1)
        {
            perror("waitpid");
            //exit(1);
        }
        else if(pid == 0)
        {
            // No child process state

        }
        else
        {
            // Recycling child process resources succeeded
        }

        while(1)  // 轮训回收指定子进程的资源
        {
            pid = waitpid(pids[2], NULL, WNOHANG);
            if(pid <= 0)
            {
                continue;
            }
            else
            {
                break;
            }
        }

        //阻塞回收子进程资源
        pid = waitpid(pids[3], NULL, WNOHANG);
        if(pid == -1)
        {
            perror("waitpid");
            //exit(0);
        }
        else
        {
            printf("blocking prarent proces to recycle child process resources succeeded\n");
        }

        //回收剩下的所有子进程
        for( i = 0; i < CHILD_PROCESS_NUM; i++)
        {
            waitpid(pids[i], NULL, WUNTRACED);
        }
    }
    else
    {
        // child
        //sleep(100 + i % 10);
    }


}
