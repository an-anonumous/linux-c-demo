#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/epoll.h>
#include <fcntl.h>

#define SERVER_IP ("10.168.57.226")
#define SERVER_POT (9988)

#define BUFFER_SIZE (64)

int main()
{
    // 创建socket文件
    // int socket(int domain, int type, int protocol);
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1)
    {
        perror("socket");
        exit(-1);
    }

    // 允许端口复用
    int opt = 1, ret;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 初始化服务器本地地址
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_POT);

    // int inet_pton(int af, const char *src, void *dst);
    ret = inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr.s_addr);
    if (ret != 1)
    {
        perror("inet_pton");
        exit(-1);
    }

    // 本地socket文件绑定地址
    // int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    ret = bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret != 0)
    {
        perror("bind");
        exit(-1);
    }

    // 开始监听
    // int listen(int sockfd, int backlog);
    ret = listen(server_fd, 128);
    if (ret != 0)
    {
        perror("listen");
        exit(-1);
    }
    char buffer[BUFFER_SIZE] = {0}; // 读写缓冲区

    //  creates an epoll(7) instance 即一个红黑树
    // int epoll_create(int size);
    int epfd = epoll_create(1024);
    if (epfd == -1)
    {
        perror("epoll_create" + __LINE__);
        exit(-1);
    }

    // 添加对服务器端socket文件的监听
    struct epoll_event server_event;
    server_event.data.fd = server_fd;

    /*server_event.events = EPOLLIN;*/
    server_event.events = EPOLLIN;

    // int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
    ret = epoll_ctl(epfd, EPOLL_CTL_ADD, server_fd, &server_event);
    if (ret != 0)
    {
        perror("epoll_ctl" + __LINE__);
        exit(1);
    }

    struct epoll_event events[FD_SETSIZE];

    // 循环监听服务器的套接字文件和已经建立连接的accept返回的客户端套接字文件，每次循环处理一个请求
    while (1)
    {
        // int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
        int ready_num = epoll_wait(epfd, events, FD_SETSIZE, -1);
        if (ready_num == -1)
        {
            perror("epoll_wait");
            exit(-1);
        }
        else if (ready_num == 0)
        {
            continue;
        }

        struct sockaddr_in new_clinet_addr;
        socklen_t new_clinetaddr_len;
        char client_ip[16] = {0};
        int clinet_port;

        int i = 0;
        for (i = 0; i < ready_num; i++)
        {
            // 先检查是否有新的连接请求到达
            if (events[i].data.fd == server_fd && events[i].events == EPOLLIN)
            {
                // int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
                int new_clinet_fd = accept(server_fd, (struct sockaddr *)&new_clinet_addr, &new_clinetaddr_len); // 此时不会阻塞
                if (new_clinet_fd == -1)
                {
                    perror("accept");
                    exit(-1);
                }

                //* 修改new_clinet_fd为非阻塞读
                int flag = fcntl(new_clinet_fd, F_GETFL);
                flag |= O_NONBLOCK;
                fcntl(new_clinet_fd, F_SETFL, flag);

                struct epoll_event new_clinet_event;
                new_clinet_event.data.fd = new_clinet_fd;
                new_clinet_event.events = EPOLLIN | EPOLLET; /* ET 边沿触发，默认是水平触发 */

                // 将新的请求添加到监控集合
                ret = epoll_ctl(epfd, EPOLL_CTL_ADD, new_clinet_fd, &new_clinet_event);
                if (ret != 0)
                {
                    perror("epoll_ctl" + __LINE__);
                    exit(1);
                }

                //const char *inet_ntop(int af, const void *src, char* dst, socklen_t size);
                char * p = inet_ntop(AF_INET, &new_clinet_addr.sin_addr, client_ip, 16);
                if (!p)
                {
                    perror("inet_ntop");
                    exit(-1);
                }
                clinet_port = ntohs(new_clinet_addr.sin_port);
                printf("Successfully received connection request from client %s:%d \n", client_ip, clinet_port);
            }

            // 已建立连接的客户端发送消息
            if (events[i].data.fd != server_fd && events[i].events == EPOLLIN)
            {
                //  读走客户端发送的所有数据
                while (1)
                {
                    int size = read(events[i].data.fd, buffer, BUFFER_SIZE - 1);

                    if (size == -1)
                    {
                        break;
                    }

                    ret = write(STDOUT_FILENO, buffer, size); // 显示
                    if (ret == -1)
                    {
                        perror("write");
                        exit(-1);
                    }

                    // 转大写后写回客户端
                    int j = 0;
                    for (j = 0; j < size; j++)
                    {
                        if (isalpha(buffer[j]))
                        {
                            buffer[j] = toupper(buffer[j]);
                        }
                    }
                    ret = write(events[i].data.fd, buffer, size);
                    if (ret == -1)
                    {
                        perror("write");
                        exit(-1);
                    }
                    memset(buffer, 0, BUFFER_SIZE);
                } //read
            }     //if
        }
    }
}
