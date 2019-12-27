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
#include <pthread.h>

/*
多线程版并发服务器
*/

#define SERVER_IP ("10.168.57.226")
#define SERVER_POT (9988)

struct MyStruct
{
    struct sockaddr_in client_addr;
    int client_fd;
};

void *fun(void *para)
{
    struct MyStruct *parg = (struct MyStruct *)para;
    int client_fd = parg->client_fd;

    uint16_t clinet_port = ntohs(parg->client_addr.sin_port);

    char client_ip[16] = {0};
    // const char *inet_ntop(int af, const void *src, char *dst, socklen_t size); // ->优先级比&高
    const char *p = inet_ntop(AF_INET, (struct sockaddr *)&parg->client_addr.sin_addr.s_addr, client_ip, 16);
    if (p == NULL)
    {
        perror("inet_ntop");
        exit(-1);
    }

    printf("%s:%d connected successfully, %d clinet_fd=%d \n", client_ip, clinet_port, getpid(), parg->client_fd);

    char buffer[1024] = {0};
    int len = -1;
    while (1)
    {
        len = read(client_fd, buffer, 1023); // ssize_t read(int fd, void *buf, size_t count);
        if (len == 0)
        { //客户端已关闭
            close(client_fd);
            exit(-1);
        }
        else if (len < 0)
        {
            perror("read");
            exit(-1);
        }
        printf("%d-%ld  from %s:%d   %s  \n", getpid(), pthread_self(), client_ip, clinet_port, buffer);
        for (int i = 0; i < strlen(buffer); i++)
        {
            if (isalpha(buffer[i]))
            {
                buffer[i] = toupper(buffer[i]);
            }
        }
        // ssize_t write(int fd, const void *buf, size_t count);
        int size = write(client_fd, buffer, len);
        if (size <= 0)
        {
            perror("write");
            exit(-1);
        }
        bzero(buffer, 1024);
    }

    free(para);
    pthread_exit(NULL);
}

int main()
{
    // int socket(int domain, int type, int protocol);
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        perror("socket");
        exit(-1);
    }

    // 允许端口复用
	int opt = 1;
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 初始化server 地址
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr)); // void explicit_bzero(void *s, size_t n);
    server_addr.sin_family = AF_INET;

    // int inet_pton(int af, const char *src, void *dst);
    int ret = inet_pton(AF_INET, SERVER_IP, (struct sockeraddr *)&server_addr.sin_addr.s_addr);
    if (ret != 1)
    {
        perror("inet_pton");
        exit(-1);
    }

    server_addr.sin_port = htons(SERVER_POT); // 主机字节序转为网络字节序

    // int bind( int sockfd, const struct sockaddr* addr, socklen_t addrlen );
    ret = bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret != 0)
    {
        perror("bind");
        exit(-1);
    }

    // int listen(int sockfd, int backlog);
    ret = listen(server_fd, 128);
    if (ret != 0)
    {
        perror("listen");
        exit(-1);
    }

    struct sockaddr_in client_addr; // 请求链接的客户端地址信息
    socklen_t client_addr_size = sizeof(client_addr);

    int client_fd;
    pid_t pid;
    while (1) //服务器循环接受请求，每个链接创建一个进程
    {
        // int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_size);
        if (client_fd == -1)
        {
            perror("accept");
            exit(-1);
        }

        pthread_t thread_id;
        struct MyStruct *p = (struct MyStruct *)malloc(sizeof(struct MyStruct));
        memset(p, 0, sizeof(struct MyStruct));
        p->client_fd = client_fd;
        p->client_addr = client_addr;

        // 创建新线程
        // int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
        ret = pthread_create(&thread_id, NULL, fun, p);
        if (ret != 0)
        {
            perror("pthread_create");
            exit(-1);
        }
        // int pthread_detach(pthread_t thread);
        ret = pthread_detach(thread_id);
        if (ret != 0)
        {
            perror("pthread_detach");
            exit(-1);
        }

    } // while

    pthread_exit(NULL);
}