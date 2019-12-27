#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char * argv[])
{
    char path[1024] = {0};

    //       char *getcwd(char *buf, size_t size);
    char * p =    getcwd(path, 1024);
    if(p == NULL)
    {
        perror("getcwd");
        exit(-1);
    }
    printf("current working dirctory is : %s \n", path);

    p =    getcwd(NULL, 0);
    if(p == NULL)
    {
        perror("getcwd");
        exit(-1);
    }
    printf("current working dirctory is : %s \n", path);
    free(p);
}
