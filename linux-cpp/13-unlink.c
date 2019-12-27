#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc, char * argv[])
{
    if(argc != 2)
    {
        printf("./myunlink file \n");
    }

    //int unlink(const char *pathname);
    int res = unlink(argv[1]);
    if (res == -1)
    {
        perror("failed to unlink soft link.\n");
        exit(-1);
    }
}
