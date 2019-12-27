#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>



int main(int argc, char * argv[])
{
    if(argc != 3)
    {
        printf("invalid parameters please input like :\n ./myrename oldname newname");
        exit(-1);
    }

    //  ./myrename oldname newname
    //   int rename(const char *oldpath, const char *newpath);
    int res =    rename(argv[1], argv[2]);
    if(res == -1)
    {
        perror("failed to rename the file. \n");
        exit(-1);
    }

}
