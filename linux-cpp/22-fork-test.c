#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    puts("Night gathers, and now my watch begins. \n ");

    //pid_t fork(void);
    pid_t pid = fork();
    if(pid == -1)
    {
        //failed to create child process
        perror("fork");
        exit(10);
    }
    else if(pid == 0)
    {
        printf("this is child process,pid = %d,ppid=%d\n", getpid(), getppid());
    }
    else
    {

        printf("this is parent process,pid = %d,ppid=%d\n the result of fork is %d\n", getpid(), getppid(), pid);
    }

    puts("It shall not end until my death. \n");
    /*
     * Night gathers, and now my watch begins.
     *  this is parent process,pid = 31659,ppid=2722
     *   the result of fork is 31660
     *   It shall not end until my death.
     *
     *   this is child process,pid = 31660,ppid=31659
     *   It shall not end until my death.
     *
     */
}
