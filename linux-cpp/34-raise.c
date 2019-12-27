#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int main()
{
    int i = 0;
    while(1)
    {
        printf("%d\n", i++);
        if(i == 3276 )
        {
            int res = raise(SIGINT);
            if(res == -1)
            {
                perror("raise");
                exit(-1);
            }
        }
    }
}
