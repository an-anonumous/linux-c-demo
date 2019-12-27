#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define FILE_SIZE (1024*1024*1024)
#define LOOP_TIMES (1024*1024)

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
        perror(" mmap");
        exit(-1);
    }

    sleep(3);
    for(int i = 0; i < LOOP_TIMES; i++)
    {

        //  void *memcpy(void *dest, const void *src, size_t n);
        memcpy(mptr, &i, sizeof(i));
        sleep(1);
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
