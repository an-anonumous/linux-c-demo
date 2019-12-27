#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define THREAD_NUM (30)

void * fun1(void * para)
{
    int j = 0;
    sleep(1);
    while (1)
    {
        j++;
        if ((long)para % 2)
        {
            printf("i am the %ld thread \n", (long)para);
            sleep(4);
        }
        else
        {
            if ((long)para % 3 == 0)
            {
                pthread_exit(para);
            }
            else if ((long)para % 5 == 0)
            {
                return para;
            }
            else
            {
                if (j == 30)
                {
                    pthread_testcancel();
                }
                //do nothing
            }
        }
    }
}

int main()
{
    pthread_t thread, subThread[THREAD_NUM] = {0};

    int ret;
    for (int i = 0; i < THREAD_NUM; i++)
    {
        // int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
        ret = pthread_create(&thread, NULL, fun1, (void *)i);
        if (ret != 0 )
        {
            printf("failed to create new thread %d ,error is %s", i, strerror(ret));

        }
        else
        {
            subThread[i] = thread;
            printf("Successfully created thread %d ,the id is %d\n", i, thread);
        }

        if (i % 7 == 0)
        {
            ret = pthread_detach(subThread[i]);    // 状态分离的线程会自己回收资源
            if (ret != 0)
            {
                printf("faild to pthread_detach the thread %d ,err is %s \n", i, strerror(ret));
            }
        }
    }

    puts("create threads finished.");

    void * retval;
    for (int i = 0; i < THREAD_NUM; i++)
    {
        if (!(i % 10 == 0 || i % 6 == 0))
        {
            ret = pthread_cancel(subThread[i]);
            if (ret != 0)
            {
                printf("faild to pthread_cancel the thread %d ,err is %s \n", i, strerror(ret));
            }
        }

        if(i % 7 != 0 )
        {
            ret = pthread_join(subThread[i], &retval);
            if (ret != 0)
            {
                printf("faild to pthread_join the thread %d ,err is %s \n", i, strerror(ret));
            }
            else
            {
                printf("Successfully reclaimed child thread resources.i= %d ,the thread returned with value %d \n", i, (int)retval);
            }
        }
    }
}
