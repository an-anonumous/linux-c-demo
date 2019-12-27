#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char * argv[])
{
    if(argc != 3)
    {
        printf("please input like:\n ./mymkdir newdir 0777\n");
        exit(-1);
    }

    //qingtian@my-server:~$ ls -l linux-cpp/ -d
    //drwxrwxr-x 3 qingtian root 4096 11月 21 20:50 linux-cpp/

    system("ls -ld .");

    //long int strtol(const char *nptr, char **endptr, int base);
    int mod = strtol(argv[2], NULL, 8);
    if(mod == LONG_MIN || mod == LONG_MIN)
    {
        perror("strtol\n");
        exit(-1);
    }

    //int mkdir(const char *pathname, mode_t mode);
    int res =    mkdir(argv[1], mod);
    if(res == -1)
    {
        perror("mkdir \n");
        exit(-1);
    }

    system("ls -ld");
    //./mymkdir newdir 751
    //drwxr-x--x 2 qingtian qingtian 4096 11月 21 21:23 newdir/
}
