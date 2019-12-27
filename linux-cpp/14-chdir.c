#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char * argv[])
{
    if(argc < 2)
    {
        printf("a.out dir\n");
        exit(1);
    }

    system("pwd \n");

    //int chdir(const char *path);
    int res = chdir(argv[1]);
    if (res == -1)
    {
        perror("failed to unlink soft link.\n");
        exit(-1);
    }

    system("pwd \n");

}
