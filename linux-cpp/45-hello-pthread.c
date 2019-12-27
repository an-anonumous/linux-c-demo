#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

void * start_routine(void * ptr)
{
    while (1)
    {
        printf("subthread: mypid = %d ,mythread id = %ld \n", getpid(), pthread_self());
        sleep(1);
    }
}

int main()
{
    // pthread_t pthread_self(void);
    printf("main thread: mypid = %d ,mythread id = %ld \n", getpid(), pthread_self());

    pthread_t threadID;
    // int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
    int ret = pthread_create(&threadID, NULL, start_routine, NULL);
    if (ret != 0)
    {
        printf("create new thread failed!\n");
        exit(ret);
    }

    printf("main thread: mypid = %d ,mythread id = %ld \n", getpid(), pthread_self());

    //sleep(5);

    puts("end of main thread");
    pthread_exit(&ret); // after the end of main thread ,subthreads can continue to run.

}
