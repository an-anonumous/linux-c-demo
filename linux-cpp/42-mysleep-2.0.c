/*
*简单模拟sleep函数
*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>

typedef void (*sighanler_t)(int);

// 信号处理函数
void fun1(int para)
{
    for (int i = 0; i < 10; i++)
    {
        printf("handling %d \n", para);
    }
}

int mysleep(int secs)
{
    sigset_t myset, oldset, tempset;

    //                          注册信号处理函数
    //sighandler_t signal(int signum, sighandler_t handler);
    sighanler_t res = signal(SIGALRM, fun1);
    if (SIG_ERR == res)
    {
        perror("signal");
        exit(-1);
    }

    //                         先屏蔽闹钟信号 SIGALRM
    int ret = sigemptyset(&myset);
    if (ret == -1)
    {
        perror("sigemptyset");
    }

    ret = sigaddset(&myset, SIGALRM);
    if (ret == -1)
    {
        perror("sigaddset");
    }

    //int sigprocmask(int how, const sigset_t * set, sigset_t * oldset);
    ret = sigprocmask(SIG_BLOCK, &myset, &oldset);
    if (ret == -1)
    {
        perror("sigprocmask");
    }

    //                          定闹钟
    //unsigned int alarm(unsigned int seconds);
    alarm(secs);

    sleep(secs + 2);     //模拟在此处被切换下CPU，下次再获得CPU时已经过了闹钟定的时间

    //                           挂起并在挂起期间取消对	SIGALRM 信号的屏蔽
    tempset = oldset;
    ret = sigdelset(&tempset, SIGALRM);
    if (ret == -1)
    {
        perror("sigdelset");
    }

    //int sigsuspend(const sigset_t *mask)
    ret = sigsuspend(&tempset);                     //取消SIGALRM的屏蔽后挂起进程
    if (ret == -1 && errno == EINTR)
    {
        printf("sigsuspend returned");
    }

    ret = sigprocmask(SIG_BLOCK, &oldset, NULL);   //恢复默认的阻塞信号集
    if (ret == -1)
    {
        perror("sigprocmask");
    }

    res = signal(SIGALRM, NULL);  //恢复SIGALRM信号的处理方式
    if (SIG_ERR == res)
    {
        perror("signal");
        exit(-1);
    }

}

int main()
{
    for (int i = 1; i < 1000000; i++)   //防止alarm（0）没设置闹钟却挂起了进程
    {
        printf("i=%d\n", i);
        printf("less sleeped %d s \n", mysleep(i % 3+1));
    }
}
