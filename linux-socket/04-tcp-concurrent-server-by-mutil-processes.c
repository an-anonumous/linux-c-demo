#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <strings.h>
#include <sys/wait.h>
#include <ctype.h>

/*
多进程版并发服务器
*/

#define SERVER_IP ("10.168.57.226")
#define SERVER_POT (9988)

// 回收僵尸子进程
void recycle_child_process(int para)
{
    printf("%d\n",para);
    
    // pid_t waitpid(pid_t pid, int *wstatus, int options);
    while (waitpid( -1, NULL, WNOHANG ) > 0)
    {
        // 在信号处理期间默认屏蔽本信号，当有多个新的子进程发送SIGCHLD信号，只记录一次，所以要循环处理掉所有已经结束的子进程
    }
}

int main()
{
    // 注册信号处理函数
    typedef void (*sighandler_t)(int);
    // sighandler_t signal( int signum, sighandler_t handler );
    sighandler_t pre_handler = signal(SIGCHLD, recycle_child_process ); // 子进程死亡时会给父进程发SIGCHLD信号
    if (SIG_ERR == pre_handler)
    {
        perror("signal");
        exit( -1 );
    }


    // int socket(int domain, int type, int protocol);
    int server_fd = socket(AF_INET, SOCK_STREAM ,0);
    if (server_fd == -1)
    {
        perror( "socket" );
        exit( -1 );
    }

    // 允许端口复用
	int opt = 1;
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 初始化server 地址
    struct sockaddr_in server_addr;
    bzero(&server_addr,sizeof(server_addr));  // void explicit_bzero(void *s, size_t n);
    server_addr.sin_family = AF_INET;
    
    // int inet_pton(int af, const char *src, void *dst);
    int ret = inet_pton(AF_INET,SERVER_IP,(struct sockeraddr *)&server_addr.sin_addr.s_addr);
    if (ret != 1)
    {
        perror("inet_pton");
        exit( -1 );
    }

    server_addr.sin_port = htons(SERVER_POT);     // 主机字节序转为网络字节序

    // int bind( int sockfd, const struct sockaddr* addr, socklen_t addrlen );
    ret = bind(server_fd,&server_addr,sizeof(server_addr));
    if (ret != 0)
    {
        perror("bind");
        exit(-1);
    }

    // int listen(int sockfd, int backlog);
    ret = listen(server_fd,128);
    if (ret != 0)
    {
        perror( "listen" );
        exit( -1 );
    }

    struct sockaddr_in client_addr; // 请求链接的客户端地址信息
    socklen_t client_addr_size = sizeof(client_addr);

    int client_fd;
    pid_t pid;
    while (1) //服务器循环接受请求，每个链接创建一个进程
    {
        // int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
        client_fd = accept(server_fd,(struct sockaddr_in *)&client_addr,&client_addr_size);
        if (client_fd == -1)
        {
            perror("accept");
            exit( -1 );
        }
        pid = fork();
        if (pid < 0)
        {
            perror("fork");
            exit( -1 );
        }
        else if (pid == 0)
        {
            // 子进程
            break;
        }
        else
        {
            // 父进程
        }

    } // while

    if (pid == 0)
    {
        close(server_fd);

        char client_ip[16] = { 0 };
        uint16_t clinet_port = ntohs(client_addr.sin_port);

        // const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
        const char* p = inet_ntop(AF_INET,(struct sockaddr *)&client_addr.sin_addr.s_addr,client_ip,16);
        if (p == NULL)
        {
            perror("inet_ntop");
            exit( -1 );
        }

        printf("%s:%d connected successfully, %d clinet_fd=%d \n",client_ip,clinet_port,getpid(),client_fd);

        char buffer[1024] = { 0 };
        int len = -1;
        while (1)
        {
            len = read( client_fd,buffer,1023);  // ssize_t read(int fd, void *buf, size_t count);
            if (len == 0 )
            {  //客户端已关闭
                close( client_fd ); 
                exit( -1 );
            }
            else if ( len < 0)
            {
                perror("read");
                exit( -1 );
            }
            printf("%d  from %s:%d   %s  \n",getpid(),client_ip,clinet_port,buffer);
            for (int i=0;i<strlen(buffer);i++)
            {
                if (isalpha(buffer[i]))
                {
                    buffer[i] = toupper( buffer[i] );
                }
            }
            // ssize_t write(int fd, const void *buf, size_t count);
            int size = write( client_fd, buffer, len);
            if (size <= 0)
            {
                perror("write");
                exit( -1 );
            }
            bzero( buffer, 1024 );
        }
    }
}