#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>


int main()
{
	char FileName[256]={0};
	printf("please input the file name \n");
	scanf("%s",FileName);
	printf("FileName=%s",FileName);

	//open the file.
	int fd = open(FileName,O_RDWR|O_CREAT,0664);
	if(fd  == -1)
	{
		perror("failed to open the file specified by FileName .");
		exit(-1);
	}

	//off_t lseek(int fd, off_t offset, int whence);
	//get the file size 
       int offsets =lseek(fd,0,SEEK_END);
       printf("the size of the file denoted by %s is %d \n",FileName,offsets);

       //扩展文件
       offsets = lseek(fd,2048,SEEK_END);
       printf("the size of the file denoted by %s is %d after truncated by lseek \n",FileName,offsets);

       //write to the file.
       int wsize = write(fd,"hello hole",sizeof("hello hole"));
       if(wsize == -1)
       {
	       perror("some errors happened when write to file.");
	       exit(-1);
       }

       //close file
       int res = close(fd);
       if(res == -1)
       {
	       perror("fail to close the file");
	       exit(-1);
       }
}
