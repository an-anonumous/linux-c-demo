#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#define LOOP_TIMES (1024*1024)
int main()
{
    int fd = open("./temp", O_RDWR | O_CREAT | O_TRUNC, 0664);
    if(fd == -1)
    {
        perror("open");
        exit(-1);
    }

    sleep(5);
    for(long i = 0; i < LOOP_TIMES; i++)
    {
        //ssize_t write(int fd, const void *buf, size_t count);
        int res = write(fd, &i, sizeof(long));
        if(res == -1)
        {
            perror("write");
            exit(-1);
        }

        //off_t lseek(int fd, off_t offset, int whence);
        int ret = lseek(fd, 0, SEEK_SET);
        if(ret == -1)
        {
            perror("lseek");
            exit(-1);
        }
        // sleep(i%2);
    }
    close(fd);
}
