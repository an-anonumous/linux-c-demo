#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>

void fun( int para)
{
    printf("hello %d  \n", para);
}

int main()
{
    typedef void (*sighandler_t)(int);
    // sighandler_t signal(int signum, sighandler_t handler);
    sighandler_t res = signal(SIGALRM, fun);
    if(res == -1)
    {
        perror("setitimer");
        exit(-1);
    }

    struct itimerval new, old;
    new.it_interval.tv_sec = 1;
    new.it_interval.tv_usec = 100;

    //new.it_value.tv_sec = 10;
    new.it_value.tv_sec = 0;
    new.it_value.tv_usec = 123;

    // 第一次发信号为1秒100ms后，以后每个10秒123ms发一次信号
    // #include <sys/time.h>
    // int setitimer(int which, const struct itimerval *new_value,struct itimerval *old_value);
    res =   setitimer(ITIMER_REAL, &new, &old);
    if(res == -1)
    {
        perror("setitimer");
        exit(-1);
    }

    int i = 1;
    while(1)
    {
        i = i * i % i;
        if(i < 100)
        {
            i = rand() * 10;
        }
    }

}
