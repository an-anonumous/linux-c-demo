#include "my-thread-pool.h"
#include <strings.h>
#include <pthread.h>

/**
 * 线程池中每个线程的入口函数，该函数循环从任务队列中获取任务并执行
 */
void* worker_thread_fun( void* para )
{
    LOG_FUN_BEGAIN
    struct My_Thread_Pool_t* pool = (struct My_Thread_Pool_t*)para;
    if (pool == NULL)
    {
        printf( "%s:%ld::%s  invalid parameter  \n", __FILE__, __LINE__, __FUNCTION__ );
        pthread_exit( NULL );
    }

    struct my_task_t new_task;
    memset( &new_task, 0, sizeof( new_task ) );

    int ret;
    while (1)
    {
        int task_full_size = -1;
        sem_getvalue( &pool->sem_task_full_num, &task_full_size );
        if (pool->is_destroying && task_full_size == 0)
        {
            break;                  // 退出，准备被回收
        }

        // int sem_wait( sem_t * sem );
        ret = sem_wait( &pool->sem_task_full_num );              // 等待任务队列不为空，并将任务数-1
        if (ret != 0)
        {
            printf( "%s:%ld::%s  error \n", __FILE__, __LINE__, __FUNCTION__ );
            perror( "sem_wait" );
            pthread_exit( -1 );
        }

        //int pthread_mutex_lock(pthread_mutex_t *mutex);
        ret = pthread_mutex_lock( &pool->mutex_task_queue );    // 独占任务队列
        if (ret != 0)
        {
            printf( "%s:%ld::%s  error \n", __FILE__, __LINE__, __FUNCTION__ );
            perror( "sem_wait" );
            pthread_exit( -1 );
        }

        if (pool->curr_task_num > 0)
        {
            // 获取队列中第一个任务
            pool->task_queue_head = (pool->task_queue_head + 1) % pool->task_queue_len;
            new_task = pool->task_queue[pool->task_queue_head];
        }

        pool->curr_task_num--;

        // int pthread_mutex_unlock(pthread_mutex_t *mutex);
        ret = pthread_mutex_unlock( &pool->mutex_task_queue );    // 解除独占任务队列
        if (ret != 0)
        {
            printf( "%s:%ld::%s  error \n", __FILE__, __LINE__, __FUNCTION__ );
            perror( "sem_wait" );
            pthread_exit( -1 );
        }

        // int sem_post(sem_t *sem);
        ret = sem_post( &pool->sem_task_empty_num );               // 空闲任务队列空间+1

        // 执行任务
        if (new_task.fun != NULL && new_task.para != NULL)
        {
            new_task.fun( new_task.para );
        }

        //void pthread_testcancel( void );
        pthread_testcancel();                                     //  创建cancellation point
    }
    pthread_exit( 0 );
    LOG_FUN_END
}

/**
 * 管理线程入口函数
 */
void* admin_thread_fun( void* para )
{
    LOG_FUN_BEGAIN
    struct My_Thread_Pool_t* pool = (struct My_Thread_Pool_t*)para;
    int ret;
    while (1)
    {
        sleep( RESIZE_POOL_INTERVAL );
        printf( "%s:%ld::%s    \n", __FILE__, __LINE__, __FUNCTION__ );

        // int pthread_mutex_lock( pthread_mutex_t * mutex );
        ret = pthread_mutex_lock( &pool->mutex_threads_id );             // 锁住线程id数组
        if (ret != 0)
        {
            printf( "%s:%ld::%s  error \n", __FILE__, __LINE__, __FUNCTION__ );
            perror( "pthread_mutex_lock" );
            pthread_exit( -1 );
        }

        ret = pthread_mutex_lock( &pool->mutex_task_queue );           // 独占任务队列，此处为了读取任务数，读写锁更好一点
        if (ret != 0)
        {
            printf( "%s:%ld::%s  error \n", __FILE__, __LINE__, __FUNCTION__ );
            perror( "pthread_mutex_lock" );
            pthread_exit( -1 );
        }
        // 每个线程的任务数过多增加线程
        if (pool->curr_task_num / pool->curr_thread_num > MAX_TASK_NUM_PER_THREAD)
        {
            ret = pthread_mutex_unlock( &pool->mutex_task_queue );     // 解锁任务队列
            if (ret != 0)
            {
                printf( "%s:%ld::%s  error \n", __FILE__, __LINE__, __FUNCTION__ );
                perror( "pthread_mutex_unlock" );
                pthread_exit( -1 );
            }

            for (int i = 0, j = 0; i < THREADS_NUM_ADJUSTMENT_STEP; i++)
            {
                for (; j < pool->threads_id_adrr_len; j++)
                {
                    if (pool->threads_id_arr[j] == -1)
                    {
                        break;
                    }
                }
                if (j == pool->threads_id_adrr_len)
                {
                    break;
                }
                // int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
                ret = pthread_create( &pool->threads_id_arr[j], NULL, worker_thread_fun, pool );
                if (ret != 0)
                {
                    printf( "%s:%ld::%s  error \n", __FILE__, __LINE__, __FUNCTION__ );
                    perror( "pthread_create" );
                    pthread_exit( -1 );
                }
                pool->curr_thread_num++;
            }
        }

        sleep( RESIZE_POOL_INTERVAL * 2 );  // 不可能刚增加了新线程又马上减少线程数，线程id数组其他线程又不用，一直占着

        ret = pthread_mutex_lock( &pool->mutex_task_queue );           // 独占任务队列
        if (ret != 0)
        {
            printf( "%s:%ld::%s  error \n", __FILE__, __LINE__, __FUNCTION__ );
            perror( "pthread_mutex_lock" );
            pthread_exit( -1 );
        }
        // 每个线程的任务数过少减少线程
        if (pool->curr_task_num / pool->curr_thread_num < MIN_TASK_NUM_PER_THREAD)
        {
            ret = pthread_mutex_unlock( &pool->mutex_task_queue );     // 解锁任务队列
            if (ret != 0)
            {
                printf( "%s:%ld::%s  error \n", __FILE__, __LINE__, __FUNCTION__ );
                perror( "pthread_mutex_unlock" );
                pthread_exit( -1 );
            }

            for (int i = 0, j = 0; i < THREADS_NUM_ADJUSTMENT_STEP; i++)
            {
                for (; j < pool->threads_id_adrr_len; j++)
                {
                    if (pool->threads_id_arr[j] != -1)
                    {
                        break;
                    }
                }
                if (j == pool->threads_id_adrr_len)
                {
                    break;
                }
                // int pthread_cancel( pthread_t thread );
                ret = pthread_cancel( pool->threads_id_arr[j] );
                if (ret != 0)
                {
                    printf( "%s:%ld::%s  error \n", __FILE__, __LINE__, __FUNCTION__ );
                    perror( "pthread_cancel" );
                    pthread_exit( -1 );
                }
                int ret = pthread_join( pool->threads_id_arr[j], NULL );
                if (ret != 0)
                {
                    printf( "%s:%ld::%s  error \n", __FILE__, __LINE__, __FUNCTION__ );
                    perror( "pthread_join" );
                    pthread_exit( -1 );
                }
                pool->threads_id_arr[j] = -1;                              // 标记已经退出线程
                pool->curr_thread_num--;
            }
        }

        // int pthread_mutex_unlock( pthread_mutex_t * mutex );
        ret = pthread_mutex_unlock( &pool->mutex_threads_id );             // 解锁线程id数组
        if (ret != 0)
        {
            printf( "%s:%ld::%s  error \n", __FILE__, __LINE__, __FUNCTION__ );
            perror( "pthread_mutex_unlock" );
            pthread_exit( -1 );
        }
        pthread_testcancel();                                             //  创建cancellation point
    }
    LOG_FUN_END
}

/**
 * 创建线程池
 */
struct My_Thread_Pool_t* threadpool_create( int min_thr_num, int max_thr_num, int queue_max_size )
{
    LOG_FUN_BEGAIN
    int  ret;
    struct My_Thread_Pool_t* poll = NULL;
    poll = (struct My_Thread_Pool_t*)malloc( sizeof( struct My_Thread_Pool_t ) );     //申请线程池管理数据结构存储空间
    if (poll == NULL)
    {
        //printf("%s:%ld::%s    \n",__FILE__,__LINE__,__FUNCTION__);
        printf( "%s:%ld::%s   initialize failed \n", __FILE__, __LINE__, __FUNCTION__ );
        pthread_exit( -1 );
    }

    poll->min_thread_num = min_thr_num;                        // 最少线程数
    //poll->max_thread_num = max_thr_num;                        // 最大线程数
    poll->curr_thread_num = (min_thr_num + max_thr_num) / 2;   // 当前线程数
    poll->task_queue_len = queue_max_size;                     // 任务队列大小
    poll->curr_task_num = 0;                                   // 初始任务数为0
    poll->threads_id_adrr_len = max_thr_num;                   // 线程id数组大小

    poll->threads_id_arr = (pthread_t*)malloc( max_thr_num * sizeof( pthread_t ) );    //申请线程id数组
    if (poll->threads_id_arr == NULL)
    {
        printf( "%s:%ld::%s   initialize failed \n", __FILE__, __LINE__, __FUNCTION__ );
        pthread_exit( -1 );
    }
    for (int i = 0; i < poll->threads_id_adrr_len; i++)
    {
        poll->threads_id_arr[i] = -1;                          // 所有线程id初始化为-1
    }

    // 申请任务队列空间
    poll->task_queue = (struct my_task_t*)malloc( queue_max_size * sizeof( struct my_task_t ) );
    if (poll->task_queue == NULL)
    {
        printf( "%s:%ld::%s   initialize failed \n", __FILE__, __LINE__, __FUNCTION__ );
        pthread_exit( -1 );
    }
    bzero( poll->task_queue, queue_max_size * sizeof( struct my_task_t ) );    //  void bzero(void *s, size_t n);

    // int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr);
    ret = pthread_mutex_init( &poll->mutex_threads_id, NULL );                 // 初始化线程号id数组的互斥量
    if (ret != 0)
    {
        printf( "%s:%ld::%s   initialize failed \n", __FILE__, __LINE__, __FUNCTION__ );
        perror( "pthread_mutex_init" );
        pthread_exit( -1 );
    }
    ret = pthread_mutex_init( &poll->mutex_task_queue, NULL );                 // 初始化任务队列互斥量
    if (ret != 0)
    {
        printf( "%s:%ld::%s   initialize failed \n", __FILE__, __LINE__, __FUNCTION__ );
        perror( "pthread_mutex_init" );
        pthread_exit( -1 );
    }

    // int sem_init(sem_t *sem, int pshared, unsigned int value);
    ret = sem_init( &poll->sem_task_empty_num, 0, poll->task_queue_len - 1 );  // 初始化任务队列剩余空间信号量，使用数组模拟循环队列，有一个空间不用
    if (ret == -1)
    {
        printf( "%s:%ld::%s   initialize failed \n", __FILE__, __LINE__, __FUNCTION__ );
        perror( "sem_init" );
        pthread_exit( -1 );
    }
    ret = sem_init( &poll->sem_task_full_num, 0, 0 );                       // 初始化任务队列已用空间信号量
    if (ret == -1)
    {
        printf( "%s:%ld::%s   initialize failed \n", __FILE__, __LINE__, __FUNCTION__ );
        perror( "sem_init" );
        pthread_exit( -1 );
    }

    // 创建工作线程
    for (int i = 0; i < poll->curr_thread_num; i++)
    {
        // int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
        ret = pthread_create( &poll->threads_id_arr[i], NULL, worker_thread_fun, poll );
        if (ret != 0)
        {
            printf( "%s:%ld::%s   initialize failed \n", __FILE__, __LINE__, __FUNCTION__ );
            perror( "pthread_create" );
            pthread_exit( -1 );
        }
    }

    // 创建管理者线程
    ret = pthread_create( &poll->admin_thread_id, NULL, admin_thread_fun, poll );
    if (ret != 0)
    {
        printf( "%s:%ld::%s   initialize failed \n", __FILE__, __LINE__, __FUNCTION__ );
        perror( "pthread_create" );
        pthread_exit( -1 );
    }
    LOG_FUN_END
    return poll;
}

/**
 * 销毁线程池
 */
void threadpool_destroy( struct My_Thread_Pool_t* pool )
{
    LOG_FUN_BEGAIN
    if (pool == NULL)
    {
        //printf("%s:%ld::%s    \n",__FILE__,__LINE__,__FUNCTION__);
        printf( "%s:%ld::%s   invalid parameters \n", __FILE__, __LINE__, __FUNCTION__ );
        pthread_exit( -1 );
    }

    pool->is_destroying = 1;               // 不再允许添加新的任务

    int remaining_task_num = -1;           // 队列中剩余的任务数
    while (1)
    {
        sleep( 1 );

        //  int sem_getvalue(sem_t *sem, int *sval);
        sem_getvalue( &pool->sem_task_full_num, &remaining_task_num );
        if (remaining_task_num == 0)
        {
            break;                        // 每隔一秒检测一次，直到所有任务执行完，再去回收僵尸线程
        }
    }

    // 所有任务已经执行完，回收所有线程
    for (int i = 0; i < pool->threads_id_adrr_len; i++)
    {
        if (pool->threads_id_arr[i] != -1)
        {
            int ret = pthread_join( pool->threads_id_arr[i], NULL );
            if (ret != 0)
            {
                printf( "%s:%ld::%s  error \n", __FILE__, __LINE__, __FUNCTION__ );
                perror( "pthread_join" );
                pthread_exit( -1 );
            }
        }
    }

    // 撤销管理者线程
    int ret = pthread_cancel( pool->admin_thread_id );
    if (ret != 0)
    {
        printf( "%s:%ld::%s  error \n", __FILE__, __LINE__, __FUNCTION__ );
        perror( "pthread_cancel" );
        pthread_exit( -1 );
    }
    ret = pthread_join( pool->admin_thread_id, NULL );
    if (ret != 0)
    {
        printf( "%s:%ld::%s  error \n", __FILE__, __LINE__, __FUNCTION__ );
        perror( "pthread_join" );
        pthread_exit( -1 );
    }

    free( pool );
    pool = NULL;
    LOG_FUN_END
}

int threadpool_add_task( struct My_Thread_Pool_t* pool, void* (*function)(void* arg), void* arg )
{
    LOG_FUN_BEGAIN
    if (pool == NULL || arg == NULL || arg == NULL)
    {
        printf( "%s:%ld::%s   invalid arguments \n", __FILE__, __LINE__, __FUNCTION__ );
        //   pthread_exit( -1 );
        return -1;
    }
    if (pool->is_destroying)
    {
        printf( "%s:%ld::%s  this thread pool is destroying \n", __FILE__, __LINE__, __FUNCTION__ );
        return -1;
    }

    int ret;

    struct my_task_t new_task;
    new_task.fun = function;
    new_task.para = arg;

    ret = sem_wait( &pool->sem_task_empty_num );   // 等待任务队列有空的空间
    if (ret != 0)
    {
        printf( "%s:%ld::%s  error \n", __FILE__, __LINE__, __FUNCTION__ );
        perror( "sem_wait" );
        pthread_exit( -1 );
    }

    // int pthread_mutex_lock(pthread_mutex_t *mutex);
    ret = pthread_mutex_lock( &pool->mutex_task_queue );           // 独占任务队列
    if (ret != 0)
    {
        printf( "%s:%ld::%s  error \n", __FILE__, __LINE__, __FUNCTION__ );
        perror( "pthread_mutex_lock" );
        pthread_exit( -1 );
    }

    if (pool->curr_task_num < pool->task_queue_len - 1)            // 再次检测是否有剩余空间
    {
        pool->task_queue_tail = (pool->task_queue_tail + 1) % pool->task_queue_len;
        pool->task_queue[pool->task_queue_tail] = new_task;        // 添加新任务
        pool->curr_task_num++;
    }

    // int pthread_mutex_unlock(pthread_mutex_t *mutex);
    ret = pthread_mutex_unlock( &pool->mutex_task_queue );        // 解除对任务队列的独占
    if (ret != 0)
    {
        printf( "%s:%ld::%s  error \n", __FILE__, __LINE__, __FUNCTION__ );
        perror( "pthread_mutex_unlock" );
        pthread_exit( -1 );
    }

    ret = sem_post( &pool->sem_task_full_num );                  // 任务队列已用空间+1
    if (ret != 0)
    {
        printf( "%s:%ld::%s  error \n", __FILE__, __LINE__, __FUNCTION__ );
        perror( "sem_post" );
        pthread_exit( -1 );
    }

    LOG_FUN_END
    return 0;
}

