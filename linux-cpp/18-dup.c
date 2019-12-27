#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

int main()
{
    char * txt[] =
    {
        "Night gathers, and now my watch begins. \n ",
        "It shall not end until my death. \n ",
        "I shall take no wife, hold no lands, father no children. \n ",
        "I shall wear no crowns and win no glory. \n ",
        "I shall live and die at my post. \n ",
        "I am the sword in the darkness. \n ",
        "I am the watcher on the walls. \n ",
        "I am the fire that burns against the cold, the light that brings the dawn, the horn that wakes the sleepers, the shield that guards the realms of men. \n ",
        "I pledge my life and honor to the Night's Watch, for this night and all the nights to come."
    };

    //int open(const char * pathname, int flags);
    //int open(const char * pathname, int flags, mode_t mode);
    int fd1 = open("./abc.txt", O_RDWR | O_CREAT, 0644);
    if(fd1 == -1 )
    {
        perror("open");
        exit(-1);
    }

    //int dup(int oldfd);
    // int dup2(int oldfd, int newfd);
    int fd2 =    dup(fd1);
    if(fd2 == -1 )
    {
        perror("open");
        exit(-1);
    }
    printf("fd1=%d,fd2=%d\n", fd1, fd2);

    //ssize_t write(int fd, const void * buf, size_t count);
    int wsize =    write(fd1, txt[0], strlen(txt[0]));
    if(wsize == -1)
    {
        perror("write\n");
        exit(-1);
    }

    wsize =    write(fd2, txt[1], strlen(txt[1]));
    if(wsize == -1)
    {
        perror("write\n");
        exit(-1);
    }

    int fd3 = dup2(fd2, 100);
    if(fd3 == -1 )
    {
        perror("open");
        exit(-1);
    }

    wsize =    write(fd3, txt[2], strlen(txt[2]));
    if(wsize == -1)
    {
        perror("write\n");
        exit(-1);
    }

    int fd4 = open("./1234.txt", O_RDWR | O_CREAT, 0644);
    if(fd4 == -1 )
    {
        perror("open");
        exit(-1);
    }

    wsize = write(fd4, txt[3], strlen(txt[3]));
    if(wsize == -1)
    {
        perror("write\n");
        exit(-1);
    }

    int fd5 = dup2(fd1, fd4);
    if(fd5 == -1 )
    {
        perror("open");
        exit(-1);
    }

    wsize = write(fd5, txt[4], strlen(txt[4]));
    if(wsize == -1)
    {
        perror("write\n");
        exit(-1);
    }

    printf("fd1=%d,fd2=%d,fd3=%d,fd4=%d,fd5=%d\n", fd1, fd2, fd3, fd4, fd5);

    system(" cat abc.txt \n\n");
    system(" cat 1234.txt \n\n");

    close(fd1);
    close(fd2);
    close(fd3);
    int res = close(fd4);
    printf("\n res=%d \n", res);
    close(fd5);
}
