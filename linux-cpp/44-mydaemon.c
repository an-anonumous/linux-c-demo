/* 守护进程的一个例子 */

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define MY_PATH  ("/home/qingtian/")

int main()
{
    // 1. 创建子进程，父进程退出
    // 所有工作在子进程中进行形式上脱离了控制终端
    printf("\\.^O^./");

    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork");
        exit(-1);
    }
    else if (pid > 0)
    {
        puts("bye parent");
        return 0;          //正常退出
    }
    else
    {
        printf("daemon : pid =%d,parent pid = %d ,group id = %d ,session id = %d \n", getpid(), getpid(), getpgid(0), getsid(0));
    }

    // 2. 在子进程中创建新会话
    // setsid()函数
    // 使子进程完全独立出来，脱离控制
    pid_t pid2 = setsid(); //pid_t setsid(void);
    if (pid2 == -1)
    {
        perror("setsid");
        exit(-1);
    }

    // 3. 改变当前目录为根目录
    // chdir()函数
    // 防止占用可卸载的文件系统
    // 也可以换成其它路径
    int ret = chdir(MY_PATH); //int chdir(const char *path);
    if (ret == -1)
    {
        perror("chdir");
        exit(-1);
    }

    // 4. 重设文件权限掩码
    // umask()函数
    // 防止继承的文件创建屏蔽字拒绝某些权限
    // 增加守护进程灵活性
    umask(0002);

    // 5. 关闭文件描述符（0, 1, 2重定向到 / dev / null）
    // 继承的打开文件不会用到，浪费系统资源，无法卸载
    int fd = open("/dev/null", O_RDWR);
    if (fd == -1)
    {
        perror("open");
        exit(-1);
    }

    //int dup2(int oldfd, int newfd);
    ret = dup2(fd, STDIN_FILENO);
    if (ret == -1)
    {
        perror("dup2");
        exit(-1);
    }

    ret = dup2(fd, STDOUT_FILENO);
    if (ret == -1)
    {
        perror("dup2");
        exit(-1);
    }

    ret = dup2(fd, STDERR_FILENO);
    if (ret == -1)
    {
        perror("dup2");
        exit(-1);
    }

    int fd2 = open("./mydaemon-output.txt", O_RDWR | O_CREAT, 0664);
    if (fd2 == -1)
    {
        perror("open");
        exit(-1);
    }

    char line[512] = {0};
    // 6. 开始执行守护进程核心工作
    while (1)
    {
        sprintf(line, "mydaemon : pid =%d,parent pid = %d ,group id = %d ,session id = %d \n", getpid(), getpid(), getpgid(0), getsid(0));
        // ssize_t write(int fd, const void *buf, size_t count);
        ret = write(fd2, line, strlen(line));
        if (ret == -1)
        {
            perror("write");
            exit(-1);
        }
        memset(line, 0, 512);
    }

    close(fd2);
    close(fd);
    // 7. 守护进程退出处理程序模型
    return 0;
}
