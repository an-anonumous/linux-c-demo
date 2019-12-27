#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc , char * argv[])
{
        if(argc != 2)
        {
                printf("please call this process in correct formate like ./access \"file\" \n");
                exit(0);
        }

        int res = access(argv[1],F_OK);
        if(res == 0)
        {
                printf("congratulations! you file %s is exits.\n",argv[1]);
        }
        else if(res == -1)
        {
                perror("F_OK\n");
        }


        res = access(argv[1],R_OK);
        if(res == 0)
        {
                printf("congratulations! you can read the file %s .\n",argv[1]);
        }
        else if(res == -1)
        {
                perror("R_OK\n");
        }

}
