#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main()
{
    puts("哈哈");
    int pipefd[2];
    int res =    pipe(pipefd);
    if(res == -1)
    {
        perror("pipe error");
        exit(-1);
    }

    pid_t pid = fork();
    if(pid == -1 )
    {
        perror("fork error");
        exit(-1);
    }
    else if(pid == 0   )
    {
// child
        close(pipefd[1]);//close write.

        char buffer[1024] = {0};
        //ssize_t read(int fd, void *buf, size_t count);
        int ret =  read(pipefd[0], buffer, 1024);
        if(ret == -1)
        {
            perror("read error");
            exit(-1);
        }
        puts(buffer);

        //after read;
        close(pipefd[0]);

    }
    else
    {
//parent
        close(pipefd[0]);   //close read
        char str[] = "Night gathers, and now my watch begins. It shall not end until my death. I shall take no wife, hold no lands, father no children. I shall wear no crowns and win no glory. I shall live and die at my post. I am the sword in the darkness. I am the watcher on the walls. I am the fire that burns against the cold, the light that brings the dawn, the horn that wakes the sleepers, the shield that guards the realms of men. I pledge my life and honor to the Night's Watch, for this night and all the nights to come.";
        res = write(pipefd[1], str, strlen(str));
        if(res == -1)
        {
            perror("write error");
            exit(-1);
        }

        // after write
        close(pipefd[1]);
    }
}
