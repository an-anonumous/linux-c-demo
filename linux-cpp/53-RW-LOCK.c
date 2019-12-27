// linux  读写锁
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#define THREAD_NUM (26*2)

char * txt[] =
{
    "听我誓言，作吾见证：",
    "Hear my words, and bear witness to my vow.",
    "长夜将至，我从今开始守望，至死方休。",
    "Night gathers, and now my watch begins.",
    "我将不娶妻，不封地，不生子。",
    "I shall take no wife, hold no lands, father no children.",
    "我将不戴王冠，不争荣宠。",
    "I shall wear no crowns and win no glory.",
    "我将尽忠职守，生死于斯。",
    "I shall live and die at my post.",
    "我是黑暗中的利剑，",
    "I am the sword in the darkness,",
    "长城上的守卫，",
    "I am the watcher on the walls.",
    "抵御寒冷的烈焰，",
    "I am the fire that burns against the cold,",
    "破晓时分的光线，",
    "the light that brings the dawn,",
    "唤醒眠者的号角，",
    "the horn that wakes the sleepers,",
    "守护王国的坚盾。",
    "the shield that guards the realms of men.",
    "我将生命与荣耀献给守夜人，",
    "I pledge my life and honor to the Night's Watch,",
    "今夜如此，夜夜皆然。",
    "for this night, and all the nights to come."
};

char buffer[1024] = { 0 };

struct MyStruct
{
    int i;
    pthread_rwlock_t * prwlock;
};

void * fun(void * para)
{
    struct MyStruct * p = (struct MyStruct *)para;
    int ret;

    if (p->i % 2 == 0)
    {
        //w
        while (1)
        {
            sleep(rand() % 10 + 3);

            //int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);
            ret = pthread_rwlock_wrlock(p->prwlock);
            if (ret != 0)
            {
                printf("pthread_rwlock_wrlock:%s\n", strerror(ret));
                exit(-1);
            }

            memset(buffer, 0, sizeof(buffer));

            // char *strcpy(char *dest, const char *src);
            strcpy(buffer, txt[p->i / 2]);

            //int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);
            ret = pthread_rwlock_unlock(p->prwlock);
            if (ret != 0)
            {
                printf("pthread_rwlock_unlock:%s\n", strerror(ret));
                exit(-1);
            }
        }
    }
    else
    {
        // r
        while (1)
        {
            sleep(rand() % 5 + 2);

            //int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
            ret = pthread_rwlock_rdlock(p->prwlock);
            if (ret != 0)
            {
                printf("pthread_rwlock_rdlock:%s\n", strerror(ret));
                exit(-1);
            }

            puts(buffer);

            // int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);
            ret = pthread_rwlock_unlock(p->prwlock);
            if (ret != 0)
            {
                printf("pthread_rwlock_unlock:%s\n", strerror(ret));
                exit(-1);
            }
        }
    }
}


int main()
{
    // 读写锁
    pthread_rwlock_t * prwlock = (pthread_rwlock_t *)malloc(sizeof(pthread_rwlock_t));
    if (prwlock == NULL)
    {
        printf("falied to malloc pthread_rwlock_t \n ");
    }

    pthread_rwlockattr_t * prwlockattr = (pthread_rwlockattr_t *)malloc(sizeof(pthread_rwlockattr_t));
    if (prwlock == NULL)
    {
        printf("falied to malloc pthread_rwlockattr_t \n ");
    }

    /*int pthread_rwlockattr_init(pthread_rwlockattr_t * attr);*/
    int ret = pthread_rwlockattr_init(prwlockattr);
    if (ret != 0)
    {
        printf("pthread_rwlockattr_init:%s\n", strerror(ret));
    }

    // int  pthread_rwlockattr_setkind_np(pthread_rwlockattr_t  *attr,  int  pref);
    // ret = pthread_rwlockattr_setkind_np(prwlockattr, PTHREAD_RWLOCK_PREFER_READER_NP);
    // ret = pthread_rwlockattr_setkind_np(prwlockattr, PTHREAD_RWLOCK_PREFER_WRITER_NP);
    ret = pthread_rwlockattr_setkind_np(prwlockattr, PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP);
    if (ret != 0)
    {
        printf("pthread_rwlockattr_setkind_np:%s\n", strerror(ret));
    }

    //int pthread_rwlock_init(pthread_rwlock_t * restrict rwlock, const pthread_rwlockattr_t * restrict attr);
    ret = pthread_rwlock_init(prwlock, prwlockattr);
    //int ret = pthread_rwlock_init(prwlock, NULL);
    if (ret != 0)
    {
        printf("pthread_rwlock_init:%s\n", strerror(ret));
    }


    //  创建线程 双数 写 ；单数 读

    pthread_attr_t attr;

    // int pthread_attr_init(pthread_attr_t * attr);
    ret = pthread_attr_init(&attr);
    if (ret != 0)
    {
        printf("pthread_attr_init:%s\n", strerror(ret));
    }

    // int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);
    ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    if (ret != 0)
    {
        printf("pthread_attr_setdetachstate:%s\n", strerror(ret));
    }

    pthread_t threadID;
    for (int i = 0 ; i < THREAD_NUM; i++)
    {
        struct MyStruct * p = (struct MyStruct *)malloc(sizeof(struct MyStruct));
        if (p == NULL)
        {
            printf(" failed to call malloc \n");
        }

        p->i = i;
        p->prwlock = prwlock;

        // int pthread_create(pthread_t * thread, const pthread_attr_t * attr, void* (*start_routine) (void*), void* arg);
        ret = pthread_create(&threadID, &attr, fun, (void *)p);
        if (ret != 0)
        {
            printf("pthread_create:%s\n", strerror(ret));
        }

        //free();
    }

    // int pthread_attr_destroy(pthread_attr_t *attr);
// 	ret = pthread_attr_destroy(&attr);
// 	if (ret != 0)
// 	{
// 		printf("pthread_attr_destroy:%s\n", strerror(ret));
// 	}

    //int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr);
    // pthread_rwlockattr_destroy(prwlockattr);

    //int pthread_rwlock_destroy(pthread_rwlock_t * rwlock);
// 	ret = pthread_rwlock_destroy(prwlock);
// 	if (ret != 0)
// 	{
// 		printf("pthread_rwlock_destroy:%s\n", strerror(ret));
// 	}

    // free(prwlockattr);
    // free(prwlock);

    pthread_exit(0);
}
