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

    int fd = open("./temp", O_RDWR | O_CREAT, 0664);
    if(fd == -1)
    {
        perror("open");
        exit(-1);
    }

    long temp;
    //for(long i = 0; i < LOOP_TIMES * 1024; i++)
    while(1)
    {
        // ssize_t read(int fd, void *buf, size_t count);
        int ret = read(fd, &temp, sizeof(long));
        if(ret == -1)
        {
            perror("read");
            exit(-1);
        }
        printf("%ld\n", temp);

        //off_t lseek(int fd, off_t offset, int whence);
        ret = lseek(fd, 0, SEEK_SET);
        if(ret == -1)
        {
            perror("lseek");
            exit(-1);
        }
    }
    close(fd);
}
