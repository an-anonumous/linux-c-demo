#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

int main()
{
//int mkfifo(const char *pathname, mode_t mode);
    int res = mkfifo("/home/qingtian/linux-cpp/my-fifo.p", 0664);
    if(res == -1)
    {
        perror("mkfifo");
        exit(-1);
    }


    //int open(const char *pathname, int flags);
    int wfd =    open("/home/qingtian/linux-cpp/my-fifo.p", O_WRONLY);
    if(wfd == -1)
    {
        perror("open fifo");
        exit(-1);
    }
    char buffer[] = "Night gathers, and now my watch begins. It shall not end until my death. I shall take no wife, hold no lands, father no children. I shall wear no crowns and win no glory. I shall live and die at my post. I am the sword in the darkness. I am the watcher on the walls. I am the fire that burns against the cold, the light that brings the dawn, the horn that wakes the sleepers, the shield that guards the realms of men. I pledge my life and honor to the Night's Watch, for this night and all the nights to come.";
    int size = write(wfd, buffer, strlen(buffer));
    if(size == -1)
    {
        perror("write");
        exit(-1);
    }

    close(wfd);
}
