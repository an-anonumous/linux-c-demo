#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

void * start_routine(void * ptr)
{
    sleep(2);
    printf("subthread %ld : mypid = %d ,mythread id = %ld \n", (long)ptr, getpid(), pthread_self());
}

int main()
{
    // pthread_t pthread_self(void);
    printf("main thread: mypid = %d ,mythread id = %ld \n", getpid(), pthread_self());

    int i = 0;
    for (; i < 10000; i++)
    {
        pthread_t threadID;
        // int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
        int ret = pthread_create(&threadID, NULL, start_routine, (void *)i);
        if (ret != 0)
        {
            printf("create new thread failed with errno %d !\n", ret);
            exit(ret);
        }
        else
        {
            printf("main: subthread %ld was sucessfully createed \n ", threadID);
        }
    }

    printf("main thread: mypid = %d ,mythread id = %ld \n", getpid(), pthread_self());

    //sleep(5);

    puts("end of main thread");
    //  pthread_exit(&i);
}
