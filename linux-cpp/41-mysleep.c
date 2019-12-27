/*
*简单模拟sleep函数
*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>

// 信号处理函数
void fun1(int para)
{
    for(int i = 0; i < 10; i++)
    {
        printf("handling %d \n", para);
    }
}

int mysleep(int secs, int msecs)
{
    //struct sigaction
    //{
    //    void (*sa_handler)(int);
    //    void (*sa_sigaction)(int, siginfo_t *, void *);
    //    sigset_t sa_mask;
    //    int sa_flags;           // 通常设置为0，表使用默认属性
    //    void (*sa_restorer)(void);
    //};

    //注册信号处理函数
    struct sigaction act;

    act.sa_handler = fun1;
    act.sa_flags = 0;

    // int sigemptyset(sigset_t *set);  将某个信号集清0   成功：0；失败：-1
    int res = sigemptyset(&act.sa_mask);
    if (res == -1)
    {
        perror("sigemptyset");
        exit(-1);
    }

    //int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
    res = sigaction(SIGALRM, &act, NULL);
    if (res == -1)
    {
        perror("sigaction");
        exit(-1);
    }

    //==================================================================================
    /* 定时  */

    //struct itimerval
    //{
    //    struct timeval it_interval; /* Interval for periodic timer */
    //    struct timeval it_value;    /* Time until next expiration */
    //};
    //struct timeval
    //{
    //    time_t tv_sec;       /* seconds */
    //    suseconds_t tv_usec; /* microseconds */
    //};
    struct itimerval new, old;

    new.it_interval.tv_sec = 0;
    new.it_interval.tv_usec = 0;

    new.it_value.tv_sec = secs;
    new.it_value.tv_usec = msecs;

    // int setitimer(int which, const struct itimerval *new_value,struct itimerval *old_value);
    res = setitimer(ITIMER_REAL, &new, &old); //SIGALRM
    if (res == -1)
    {
        perror("setitimer\n");
        exit(-1);
    }

    printf("old: Interval=%lds+%ldms,it-value=%lds+%lds\n", old.it_interval.tv_sec, old.it_interval.tv_usec, old.it_value.tv_sec, old.it_value.tv_usec);

    /******如果当前进程在此处被切换下CPU，再次获得CPU的时候已经过了闹钟设置的时间了，进程处理完软中断，再调用pause将永久挂起******/
    
    //==================================================================================
    /*       挂起当前进程    */

    // int pause(void);
    res = pause();
    if (res == -1 && errno ==  EINTR)
    {
        printf("pause success\n");
    }

    //int getitimer(int which, struct itimerval *curr_value);
    res = getitimer(ITIMER_REAL, &old);
    if (res == -1)
    {
        perror("getitimer\n");
        exit(-1);
    }

    return old.it_value.tv_sec;
}

int main()
{
    for (int i = 0; i < 1000000; i++)
    {
        printf("i=%d\n", i);
        mysleep(1, i);
    }
}
