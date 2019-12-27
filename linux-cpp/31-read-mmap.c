#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FILE_SIZE (1024*1024*1024)

int main()
{
    int fd = open("./temp", O_RDWR | O_CREAT, 0664);
    if(fd == -1)
    {
        perror("open");
        exit(-1);
    }

    int ret = ftruncate(fd, FILE_SIZE);
    if(ret == -1)
    {
        perror("ftruncate \n");
        exit(-1);
    }

    // void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
    void * mptr =    mmap(NULL, FILE_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
    if(mptr == MAP_FAILED)
    {
        perror("mmap\n");
        exit(-1);
    }

    int temp = -1;
    while(1)
    {
        memcpy(&temp, mptr, sizeof(int));

        printf("%d \n", temp);
    }

    int res = munmap(mptr, FILE_SIZE);
    if (res == -1)
    {
        perror("munmap \n");
        exit(-1);
    }

    unlink("./temp");
    close(fd);


}
