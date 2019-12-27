#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    while(1)
    {
        printf("%d \n", alarm(rand() % 10));
        sleep(2);

    }

}
