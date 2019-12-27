#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>


int main()
{
    char * FileName = "pledge.txt";

    // int open(const char *pathname, int flags, mode_t mode);
    int fd = open(FileName, O_RDWR | O_CREAT, 0664);
    if(fd == -1)
    {
        perror("open failed\n");
        exit(-1);
    }

    //        int truncate(const char *path, off_t length);
    //        int ftruncate(int fd, off_t length);
    int ret = ftruncate(fd, 8 * 1024);
    if(ret == -1)
    {
        perror("truncate");
        exit(-1);
    }

    // void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
    /*
     * addr 映射区的首地址，(NULL内核自己决定)                length 映射区大小（4k的整数倍）
     * prot 映射区的权限（必须要有read权限）                  flags   映射区是共享(修改同步到磁盘)的还是私有的
     * fd 文件描述符                                           offset  文件从offset开始映射，必须是4k的整数倍
     */

    void * mptr = mmap(NULL, 4 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(mptr == MAP_FAILED)
    {
        perror("mmap error");
        exit(-1);
    }

    // write data
    char * contents[] =
    {
        "Night gathers, and now my watch begins.\n ", "It shall not end until my death.\n ", "shall not end until my death.\n "
    };

    //  void *memcpy(void *dest, const void *src, size_t n);
    memcpy(mptr + 512, contents[0], strlen(contents[0]));

    *((double *)(mptr + 1024)) = 3.141592653;

    int size = write(fd, contents[1], strlen(contents[1]));
    if(size == -1)
    {
        perror("write error");
        exit(-1);
    }

    //int munmap(void *addr, size_t length);
    int res = munmap(mptr, 4 * 1024);
    if(res == -1)
    {
        perror("munmap \n");
        exit(-1);
    }

    close(fd);

}
