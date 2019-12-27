#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define  THREAD_NUM (200)

pthread_mutex_t mutex;
pthread_mutexattr_t  mutexattr;

void* fun(void* para)
{
	long i = (long)para;
	char buffer[1024] = { 0 };

	while (1)
	{
		//int pthread_mutex_lock(pthread_mutex_t * mutex);
		int ret = pthread_mutex_lock(&mutex);
		if (ret != 0)
		{
			printf(" pthread_mutex_lock error is %s \n",strerror(ret));
			continue;  // 已经拥有锁的线程，再申请会报错
		}

		//printf("i am thread %ld\t%ld%ld%ld \n", pthread_self(),i, i, i);
		printf("i am thread %ld\t", pthread_self());
		usleep(100);

		printf("%ld",i);
		usleep(100);

		printf("%ld", i); 
		usleep(100);

		printf("%ld\t", i);
		usleep(100);

		puts("~~~~~~~");

		memset(buffer,0,sizeof(buffer));
		usleep(100);

		// int pthread_mutex_unlock(pthread_mutex_t * mutex);
		ret = pthread_mutex_unlock(&mutex);
		if (ret != 0)
		{
			printf(" pthread_mutex_unlock error is %s \n", strerror(ret));
		}
	}
}

int main()
{
	// 初始化 `error checking类型的mutex

	// int pthread_mutexattr_init(pthread_mutexattr_t *attr);
	int ret = pthread_mutexattr_init(&mutexattr);
	if (ret != 0)
	{
		printf("pthread_mutexattr_init ,error is %s \n", strerror(ret));
	}

	// int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int kind);
	ret = pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_ERRORCHECK_NP);
	if (ret != 0)
	{
		printf("pthread_mutexattr_settype ,error is %s \n", strerror(ret));
	}
	
	//int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr);
	ret =pthread_mutex_init(&mutex, &mutexattr);
	if (ret != 0)
	{
		printf("pthread_mutex_init ,error is %s \n", strerror(ret));
	}

	// 创建线程

	pthread_attr_t attr;

	// int pthread_attr_init(pthread_attr_t *attr);
	ret = pthread_attr_init(&attr);
	if (ret != 0)
	{
		printf("pthread_attr_init ,error is %s \n", strerror(ret));
	}

	// int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);
	ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if (ret != 0)
	{
		printf("pthread_attr_setdetachstate ,error is %s \n", strerror(ret));
	}

	pthread_t pthreadid;
	for (int i = 0; i < THREAD_NUM; i++)
	{
		//  void *malloc(size_t size);
		void * p = malloc(4096 * 4);
		if (p == NULL)
		{
			printf("malloc error \n");
		}

		// int pthread_attr_setstack(pthread_attr_t *attr, void *stackaddr, size_t stacksize);
		ret = pthread_attr_setstack(&attr,p,4096*4);
		if (ret != 0)
		{
			printf("pthread_attr_setstack ,error is %s \n", strerror(ret));
		}
		
		// int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
		ret = pthread_create(&pthreadid, &attr, fun, (void*)i);
		if (ret != 0)
		{
			printf("pthread_create ,error is %s \n", strerror(ret));
		}
	}

	pthread_exit(0);
}