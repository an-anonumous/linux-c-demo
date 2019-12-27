#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#define LOOP_TIMES (10000)

void fun1(int para)
{
    for (int i = 0; i < LOOP_TIMES; i++)
    {
        printf("catched %d \n", para);
        if(i % 3 == 0)
        {
            sleep(1);
        }
    }

    puts("finish");
}


int main()
{
    struct sigaction act;

    //struct sigaction
    //{
    //    void (*sa_handler)(int);
    //    void (*sa_sigaction)(int, siginfo_t *, void *);
    //    sigset_t sa_mask;
    //    int sa_flags;           // 通常设置为0，表使用默认属性
    //    void (*sa_restorer)(void);
    //};

    /* linux 系统实际定义 */
    // 	struct sigaction
    // 	{
    // 		/* Signal handler.  */
    // 	#if defined __USE_POSIX199309 || defined __USE_XOPEN_EXTENDED
    // 		union
    // 		{
    // 			/* Used if SA_SIGINFO is not set.  */
    // 			__sighandler_t sa_handler;
    // 			/* Used if SA_SIGINFO is set.  */
    // 			void (*sa_sigaction) (int, siginfo_t*, void*);
    // 		}
    // 		__sigaction_handler;
    // 	# define sa_handler	__sigaction_handler.sa_handler
    // 	# define sa_sigaction	__sigaction_handler.sa_sigaction
    // 	#else
    // 		__sighandler_t sa_handler;
    // 	#endif
    //
    // 		/* Additional set of signals to be blocked.  */
    // 		__sigset_t sa_mask;
    //
    // 		/* Special flags.  */
    // 		int sa_flags;
    //
    // 		/* Restore handler.  */
    // 		void (*sa_restorer) (void);
    // 	};


    //act.sa_sigaction = NULL;   //sa_sigaction和sa_handler是union中的同一个字段
    act.sa_handler = fun1;
    act.sa_flags = 0;

    // int sigemptyset(sigset_t *set);  将某个信号集清0   成功：0；失败：-1
    int res = sigemptyset(&act.sa_mask);
    if (res == -1)
    {
        perror("sigemptyset");
        exit(-1);
    }

    // int sigaddset(sigset_t *set, int signum);  将某个信号加入信号集  成功：0；失败：-1

    // 默认自动屏蔽正在处理的信号
    // res = sigaddset(&act.sa_mask, SIGINT); //ctrl +c
    // if(res == -1)
    // {
    //     perror("sigaddset");
    //     exit(-1);
    // }

    res = sigaddset(&act.sa_mask, SIGQUIT); //ctrl +"\ "
    if (res == -1)
    {
        perror("sigaddset");
        exit(-1);
    }

    res = sigaddset(&act.sa_mask, SIGTSTP); //ctrl + z
    if (res == -1)
    {
        perror("sigaddset");
        exit(-1);
    }

    //int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);)
    res = sigaction(SIGINT, &act, NULL);
    if (res == -1)
    {
        perror("sigaction");
        exit(-1);
    }

    while (1)
    {
        printf("~~~~~~~~~~~~~~~~~~~\n");
        sleep(1);
    }

}
