#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <string.h>

/*
 * 打印未决信号集
 * 01100000000000000001000000000000
 * ctrl+C   2
 * ctrl + \  3
 * ctrl+Z  20
 */
int main()
{
    sigset_t newmask, oldmaskset, pendingset;

    //将所有bit置为0
    // int sigemptyset(sigset_t *set);
    int  res =    sigemptyset(&newmask);
    if (res == -1 )
    {
        perror("sigemptyset");
        exit(-1);
    }

    //  将打算屏蔽的信号对应的位值1
    //  int sigaddset(sigset_t *set, int signum);
    res = sigaddset(&newmask, SIGINT); // ctrl+C   2
    if(res == -1)
    {
        perror("sigaddset");
        exit(-1);
    }

    res = sigaddset(&newmask, SIGQUIT); // ctrl+\  3
    if(res == -1)
    {
        perror("sigaddset");
        exit(-1);
    }

    res = sigaddset(&newmask, SIGTSTP); // ctrl+Z  20
    if(res == -1)
    {
        perror("sigaddset");
        exit(-1);
    }

    //设计进程的阻塞信号集
    // int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
    res = sigprocmask(SIG_BLOCK, &newmask, &oldmaskset);
    if(res == -1)
    {
        perror("sigprocmask");
        exit(-1);
    }

    //循环读取并打印未决信号集
    while(1)
    {
        //int sigpending(sigset_t *set);
        res = sigpending(&pendingset);
        if(res == -1)
        {
            perror("sigprocmask");
            exit(-1);
        }

        for(int i = 1; i <= 32; i++)
        {
            // int sigismember(const sigset_t *set, int signum); 判断某个信号是否在信号集中      返回值：在集合：1；不在：0；出错：-1
            res = sigismember(&pendingset, i);
            if(res == -1)
            {
                perror("sigprocmask");
                exit(-1);
            }
            else if(res == 1)
            {
                putchar('1');
            }
            else if(res == 0)
            {
                putchar('0');
            }
        }
        puts(" ");
    }

}
