#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

// 要创建的子进程个数
#define CHILD_PRO_NUM (10000)

// 信号处理函数
void fun(int para)
{
    pid_t pid;
    int wstatus;

    printf("hello %d \n", para);
    //回收子进程
    //pid_t waitpid(pid_t pid, int* wstatus, int options);
    while (waitpid(-1, &wstatus, WNOHANG) > 0)   //在信号处理期间可能会有多个子进程结束，但未决信号集只记录一次，所以要循环回收
    {
        if (WIFEXITED(wstatus))             // returns true if the child terminated normally
        {
            printf("child process exited nomally with value %d \n", WEXITSTATUS(wstatus));
        }
        else if (WIFSIGNALED(wstatus))      // returns true if the child process was terminated by a signal.
        {
            printf("the exiting of child process was caused by signal  %d \n", WEXITSTATUS(wstatus));
        }
    }
}

int main()
{
    sigset_t myset, oldset;
    // 先屏蔽SIGCHLD信号，不让父进程执行默认动作，默认动作为忽略这个信号。
    // int sigemptyset(sigset_t *set);
    int res = sigemptyset(&myset);                    // 先将信号集初始化为空
    if (res == -1)
    {
        perror("sigemptyset");
        exit(-1);
    }

    res = sigaddset(&myset, SIGCHLD);                  // 屏蔽SIGCHLD信号
    if (res == -1)
    {
        perror("sigaddset");
        exit(-1);
    }

    // int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
    res = sigprocmask(SIG_BLOCK, &myset, &oldset);     //  设置当前进程的阻塞信号集
    if (res == -1)
    {
        perror("sigprocmask");
        exit(-1);
    }

    // 注册SIGCHLD信号处理函数
    typedef void (*sighandler_t)(int);
    //sighandler_t signal(int signum, sighandler_t handler);
    sighandler_t ret = signal(SIGCHLD, fun);
    if (SIG_ERR == ret)              //#define	SIG_ERR	 ((__sighandler_t) -1)	/* Error return.  */
    {
        perror("signal");
        exit(-1);
    }

    // 解除对SIGCHLD信号的屏蔽
    res = sigdelset(&oldset, SIGCHLD);                  //int sigdelset(sigset_t *set, int signum);
    if (res == -1)
    {
        perror("sigdelset");
        exit(-1);
    }
    res = sigprocmask(SIG_SETMASK, &oldset, NULL);     // SIG_SETMASK 设置当前进程的阻塞信号集
    if (res == -1)
    {
        perror("sigprocmask");
        exit(-1);
    }

    //创建子进程
    int i = 0;
    for (; i < CHILD_PRO_NUM; i++)
    {
        pid_t pid = fork();
        if (pid == -1)
        {
            perror("fork");
            exit(-1);
        }
        else if (pid == 0)
        {
            //child
            break;
        }
        else
        {
            //parent
            puts("~");
        }
    }

    if (i == CHILD_PRO_NUM)
    {
        // 父进程死循环
        while (1)
        {
            printf("^*^O^*^\n");
            sleep(1);
        }
    }
    else
    {
        sleep(rand() % 3 + 1);
        if (i == CHILD_PRO_NUM)
        {
            // 父进程死循环
            while (1)
            {
                printf("^.*^O^*.^\n");
                sleep(1);
            }
        }
        else
        {
            sleep(rand() % 3 + 1);

            if (rand() % 2)
            {
                kill(getpid(), rand() % 30 + 1);
            }

            return rand();
        }
    }
}
