#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

int main(int argc, char * argv[])
{
    //          ./mytuncate file 4096
    if(argc != 3)
    {
        printf("invalid call formate,please use this procedure like ./mytuncate file 4096\n");
        exit(-1);
    }

    int size = strtol(argv[2], NULL, 10);
    if(size == LONG_MAX || size == LONG_MIN)
    {
        printf("failed to call strtol\n");
        exit(-1);
    }

    // cmd
    char cmd[1024] = " ls -l ";
    sprintf(cmd + strlen(" ls -l "), " %s \n", argv[1]);
    printf("before truncate \n %s \n", cmd);
    system(cmd);

    //int truncate(const char *path, off_t length);
    truncate(argv[1], size);

    printf("\n after \n");
    system(cmd);
}
