#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>

int main(int argc, char * argv[])
{
    //carete and open a temporary file.
    const char * const filename = ".mytemp";

    // int open(const char *pathname, int flags, mode_t mode);
    int fd = open(filename, O_RDWR | O_CREAT, 0664);
    if(fd == -1)
    {
        perror("open file.\n");
        exit(-1);
    }

    system(" ls -al|grep \".mytemp\" \n");

    // unlink file after the file is opened .
    int res =    unlink(filename);
    if(res == -1 )
    {
        perror("unlink");
        exit(-1);
    }

    char * content = "Night gathers, and now my watch begins. It shall not end until my death.\n ";
    res =    write(fd, content, strlen(content)); //ssize_t write(int fd, const void *buf, size_t count);)
    if(res == -1 )
    {
        perror("write");
        exit(-1);
    }
    else
    {
        printf("write into .mytemp file successfully\n");
    }

    system(" ls -alh | grep \".mytemp\" \n");
    //system(" cat \'.mytemp\' \n");

    //close the file.
    res =    close(fd);
    if(res == -1 )
    {
        perror("close");
        exit(-1);
    }

    system(" ls -al|grep \".mytemp\" \n");

}
