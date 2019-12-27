// 查看当前系统最大能创建多少个线程
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#define MY_THREAD_STAK_SIZE  (4096*4)

// 线程入口函数
void * thread_main(void * para)
{
    int i = 0;
    while (1)
    {
        i = (i + 1) % 1000;
        //sleep(1);
    }
}


int main()
{
    pthread_attr_t attr;

    //int pthread_attr_init(pthread_attr_t *attr);
    int ret = pthread_attr_init(&attr);                                  //初始化线程属性对象
    if (ret != 0)
    {
        printf("failed to call pthread_attr_init ,err is %s \n", strerror(ret));
        exit(-1);
    }

    //int pthread_attr_setdetachstate(pthread_attr_t * attr, int detachstate);
    ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);  // 设置线程分离运行
    if (ret != 0)
    {
        printf("failed to call pthread_attr_setdetachstate ,err is %s \n", strerror(ret));
        exit(-1);
    }

    void * p = malloc(MY_THREAD_STAK_SIZE);
    if (p == NULL)
    {
        printf("failed to create malloc \n");
    }

    //int pthread_attr_setstack(pthread_attr_t * attr, void* stackaddr, size_t stacksize);
    ret = pthread_attr_setstack(&attr, p, MY_THREAD_STAK_SIZE);
    if (ret != 0)
    {
        printf("failed to call pthread_attr_setstack ,err is %s \n", strerror(ret));
        exit(-1);
    }

    pthread_t newThreadId;
    int j = 0;
    while (1)
    {
        // int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
        ret = pthread_create(&newThreadId, &attr, thread_main, NULL);
        if (ret != 0)
        {
            printf("failed to call pthread_create ,err is %s \n", strerror(ret));
            exit(-1);
        }

        printf(" Successfully created the %d thread \n", j++);
    }
}
