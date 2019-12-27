#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

/*
 *复制文件
 */
int main() {
	char src[256] = { 0 };
	char dest[256] = { 0 };

	printf("please input the source file path\n");
	scanf("%s", src);

	printf("please input the destination file path\n");
	scanf("%s", dest);

	//printf("src=%s;dest=%s\n", src, dest);

	char buffer[1024 * 4] = { 0 };

	//open the source file .
	int fd1 = open(src, O_RDONLY);
	if (fd1 == -1) {
		perror("open the source file failed!");
		exit(-1);
	}

	//open the destination file .
	int fd2 = open(dest, O_RDWR | O_CREAT,0775);
	if (fd2 == -1) {
		perror("open the source file failed!");
		exit(-1);
	}

	//copy
	int size = read(fd1, buffer, sizeof(buffer));
	if (size == -1) {
		perror("open the target file");
		exit(-1);
	}
	while (size)
	{
		int wsize = write(fd2, buffer, size);
		if (wsize == -1) {
			perror("some errors happened while write buffer to the destination file. ");
			exit(-1);
		}

		size = read(fd1, buffer, sizeof(buffer));
		if (size == -1) {
			perror("open the target file");
			exit(-1);
		}
	}

	//close all files.
	int res = close(fd1);
	if (res == 0)
	{
		printf("close file %d success \n", fd1);
	}
	else if (res == -1)
	{
		printf("close file %d failed !\n", fd1);
	}
	res = close(fd2);

	if (res == 0)
	{
		printf("close file %d success \n", fd2);
	}
	else if (res == -1)
	{
		printf("close file %d failed !\n", fd2);
	}
	   
}

