#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main()
{
    puts("hello parent.");

    pid_t pid =    fork();
    if(pid == -1)
    {
        perror("fork");
        exit(-1);
    }
    else if(pid == 0)
    {
//child progresss.
//int execlp(const char *file, const char *arg, ... /* (char  *) NULL */);
        int res =   execl("/bin/ls", "-a", "-l", NULL);
        if(res == -1)
        {
            perror("execlp");
            exit(-1);
        }
    }
    else
    {
        sleep(1);
    }
}
