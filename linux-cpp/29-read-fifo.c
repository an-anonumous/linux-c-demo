#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>



int main()
{
    int fd =    open("/home/qingtian/linux-cpp/my-fifo.p", O_RDONLY);
    if(fd == -1)
    {
        perror("open fifo");
        exit(-1);
    }

    char buffer[2048] = {0};
    int res =    read(fd, buffer, 2018);
    if (res == -1)
    {
        perror("read fifo");
        exit(-1);
    }

    printf("read from fifo :\n%s", buffer);

}
