/* 多线程copy */

#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>

#define THREAD_NUM (10)

struct MyStruct
{
	void* src;
	void* dest;
	long size;
};

// 线程入口函数
void* cp_on_thread(void * para)
{
	usleep(100);
	MyStruct* param = (MyStruct*)para;
	memcpy(param->dest, param->src,param->size);
}

int main(int argc, char* argv[])
{
	//    ./mycopy src dest
	if (argc != 3)
	{
		printf("Call format error, please enter the following format: \n \t  ./mycopy src dest \n ");
		exit(0);
	}

	// 打开源文件

	//int open(const char *pathname, int flags);
	int fd = open(argv[1], O_RDWR);
	if (fd == -1)
	{
		perror("open");
		exit(-1);
	}

	// 获取源文件大小

	//off_t lseek(int fd, off_t offset, int whence);
	long fileZise = lseek(fd, 0, SEEK_END);
	if (fileZise == -1)
	{
		perror("lseek");
		exit(-1);
	}

	// 建立原文件内存映射区

	//void* mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset);
	/*
	addr 映射区的首地址，(NULL内核自己决定)            length 映射区大小（4k的整数倍），不能大于文件实际大小
	prot 映射区的权限                                  flags   映射区是共享(修改同步到磁盘)的还是私有的
	fd 文件描述符                                      offset  文件从offset开始映射，必须是4k的整数倍
	*/

	void* srcMF = mmap(NULL, fileZise, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (srcMF == (void*)-1)
	{
		perror("mmap");
		exit(-1);
	}

	// 打开或创建目标文件
	int fd2 = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, 0664);
	if (fd2 == -1)
	{
		perror("open");
		exit(-1);
	}

	// 拓展目标文件
	int ret = ftruncate(fd2, fileZise);
	if (ret == -1)
	{
		perror("open");
		exit(-1);
	}

	void* destMF = mmap(NULL, fileZise, PROT_READ | PROT_WRITE, MAP_SHARED, fd2, 0);
	if (destMF == (void*)-1)
	{
		perror("mmap");
		exit(-1);
	}

	/*              copy begin               */

	long SIZE_PER_THREAD= fileZise/THREAD_NUM;
	pthread_t tids[THREAD_NUM] = { 0 };              //记录线程号
	pthread_t threadID;
	for (int i = 0; i < THREAD_NUM; i++)
	{
		MyStruct* para = (MyStruct*)malloc(sizeof(MyStruct));
		if (para == NULL)
		{
			exit(-1);
		}

		//子线程参数
		if (i == THREAD_NUM - 1)
		{
			para->size = fileZise- i * SIZE_PER_THREAD;
			para->dest = destMF + i * SIZE_PER_THREAD;
			para->src = srcMF + i * SIZE_PER_THREAD;
		}
		else
		{
			para->dest = destMF + i * SIZE_PER_THREAD;
			para->src = srcMF + i * SIZE_PER_THREAD;
			para->size = SIZE_PER_THREAD;
		}

		//int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
		ret = pthread_create(&threadID,NULL, cp_on_thread,para);
		if (ret != 0)
		{
			printf("faild to pthread_create the thread ,err is %s \n", strerror(ret));
		}

		tids[i] = threadID;
	}

	usleep(100);
	
	// 回收所有子线程
	for (int i =0;i<THREAD_NUM;i++)
	{
		ret = pthread_join(tids[i], NULL);
		if (ret != 0)
		{
			printf("faild to pthread_join the thread ,err is %s \n", strerror(ret));
		}
	}
	
	/*              copy end                */

	// 释放原文件内存映射区

	//int munmap(void* addr, size_t length);
	ret = munmap(srcMF, fileZise);
	if (ret == -1)
	{
		perror("munmap");
		exit(-1);
	}

	// 关闭原文件文件
	close(fd);

	// 关闭目标文件缓冲区
	ret = munmap(destMF, fileZise);
	if (ret == -1)
	{
		perror("munmap");
		exit(-1);
	}

	// 关闭目标文件
	close(fd2);
}