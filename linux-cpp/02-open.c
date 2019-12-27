#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
	int fd =-1;

	//打开一个已经存在的文件
        fd =open("hello.c",O_RDWR);
	if(fd == -1){
		perror("open file failed! \n");
		exit(1);
	}

	// create new file if the specified file is not exists
	int fd2= open("my-new-file",O_RDWR|O_CREAT,0775);
	if(fd2 == -1){
		perror("create new file failed! \n");
		exit(-1);
	}

	//to ensure if the file is exists.
	int fd3= open("my-new-file2",O_RDWR|O_CREAT|O_EXCL,0775);
	if(fd3 == -1){
        	perror("create new file failed! \n");
		exit(-1);
	}

	// 将文件截断
	int fd4= open("abc",O_RDWR|O_TRUNC,0775);
	if(fd4 == -1){
        	perror("create new file failed! \n");
		exit(-1);
	}
	

	// 关闭打开的文件
	int res=close(fd);
	if(res == 0)
	{
		printf("close file %d success \n",fd);
	}
	else if(res ==-1){

		printf("close file failed !\n");

	}

	 res=close(fd2);
	if(res == 0)
	{
		printf("close file %d success \n",fd2);
	}
	else if(res ==-1){

		printf("close file failed !\n");

	}

	 res=close(fd3);
	if(res == 0)
	{
		printf("close file %d success \n",fd3);
	}
	else if(res ==-1){

		printf("close file failed !\n");

	}

	 res=close(fd4);
	if(res == 0)
	{
		printf("close file %d success \n",fd4);
	}
	else if(res ==-1){

		printf("close file failed !\n");
	}

}
