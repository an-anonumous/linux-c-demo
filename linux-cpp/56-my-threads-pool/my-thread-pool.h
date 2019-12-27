#ifndef MY_THREAD_POOL
#define MY_THREAD_POOL

#include <pthread.h>
#include <semaphore.h>

// 管理线程执行的时间间隔
#define  RESIZE_POOL_INTERVAL  (10)

// 每次增加或者减少的线程数
#define THREADS_NUM_ADJUSTMENT_STEP (5)

// 每个线程最大的任务数
#define MAX_TASK_NUM_PER_THREAD (100)

// 每个线程的最小线程数
#define  MIN_TASK_NUM_PER_THREAD (20)

// 日志
#define LOG_FUN_BEGAIN printf( "%s:%ld::%s start runing \n", __FILE__, __LINE__, __FUNCTION__ );
#define LOG_FUN_END printf( "%s:%ld::%s end runing \n", __FILE__, __LINE__, __FUNCTION__ );

struct my_task_t
{
    void* para;          // 任务的参数
    void* (*fun)(void*)  // 每个任务的回调函数
};

struct My_Thread_Pool_t
{
    pthread_t*         threads_id_arr;               // 用一个数组来存放已经创建的线程id
    int                threads_id_adrr_len;          // 存放线程id的数组大小
    int                min_thread_num;               // 线程池中最小线程数
//    int                max_thread_num;               // 线程池中最大的线程数
    int                curr_thread_num;              // 当前线程池中的线程数

    pthread_mutex_t    mutex_threads_id;             // 用于对线程threads_id_arr数组访问的互斥

    struct my_task_t*  task_queue;                   // 存放任务的队列
    int                task_queue_len;               // 任务队列的大小
    int                curr_task_num;                // 当前任务队列中实际任务数
    int                task_queue_head;              // 当前任务队列的头指针
    int                task_queue_tail;              // 任务队列的队尾指针

//     pthread_cond_t     task_queue_not_empty;         // 当任务队列由空变非空时通知其他线程领取任务
//     pthread_cond_t     task_queue_not_full;          // 当任务已满的队列有任务被取走后通知任务添加线程，开始添加任务
    sem_t              sem_task_empty_num;           // 任务队列中空闲缓冲区大小
    sem_t              sem_task_full_num;            // 任务队列中已用缓冲区大小
    pthread_mutex_t    mutex_task_queue;             // 用于任务队列的互斥,与任务队列相关的属性均使用该锁来保护

    pthread_t          admin_thread_id;              // 管理线程id
    int                is_destroying;                // 正在销毁的线程池不允许添加新的任务，所有任务执行完后kill所有线程
};

/**
 * 创建线程池
 */
struct My_Thread_Pool_t * threadpool_create( int min_thr_num, int max_thr_num, int queue_max_size );

/**
 * 销毁线程池
 */
void threadpool_destroy( struct My_Thread_Pool_t * pool );

/**
 * 往线程池中添加任务
 */
int threadpool_add_task( struct My_Thread_Pool_t* pool, void* (*function)(void* arg), void* arg );

/**
 * 线程池中每个线程的入口函数，该函数循环从任务队列中获取任务并执行
 */
void* worker_thread_fun( void* );

/**
 * 管理线程入口函数
 */
void* admin_thread_fun(void * para);

#endif // !MY_THREAD_POOL

