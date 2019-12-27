#include "my-thread-pool.h"
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#define TASK_NUM (10000)

// 任务函数
void* task_fun( void * arg)
{
    LOG_FUN_BEGAIN
    long v = (long)arg;
    for (int i=1;i<5;i++)
    {
        usleep( 1000 );
        printf("my thread id = %ld ,task : %ld \n",pthread_self(),v);
    }
    LOG_FUN_END
}

int main()
{
    LOG_FUN_BEGAIN
    // struct My_Thread_pool_t * threadpool_create( int min_thr_num, int max_thr_num, int queue_max_size );
    struct My_Thread_Pool_t* pool = threadpool_create(20,50,200);
    //struct My_Thread_Pool_t* pool = threadpool_create(5,10,20);
    if (pool == NULL)
    {
        printf( "%s:%ld::%s  failed  \n", __FILE__, __LINE__, __FUNCTION__ );
    }

    int ret = -1;
    for (int i =0;i<TASK_NUM;i++)
    {
        // int threadpool_add_task( struct My_Thread_pool_t* pool, void* (*function)(void* arg), void* arg );
        ret = threadpool_add_task(pool,task_fun,i+1);
    }

    // void threadpool_destroy( struct My_Thread_pool_t * pool );
    threadpool_destroy(pool);

    pthread_exit(0);
    LOG_FUN_END
}