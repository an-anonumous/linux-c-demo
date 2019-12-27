#include <stdio.h>
#include <stdlib.h>


int main(int argc, char * argv[])
{
    if(argc != 2)
    {
        printf(" ./getenv PATH\n");
    }
    char * penv =    getenv(argv[1]);
    puts(penv);

}
