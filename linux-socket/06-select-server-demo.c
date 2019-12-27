#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <errno.h>

#define SERVER_IP ("10.168.57.226")
#define SERVER_POT (9988)

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

    int curr_max_fd = server_fd; // 初始化最大文件描述符

    int watched_client_fds[FD_SETSIZE]; // 记录已经被监听的客户端文件描述符
    for (int i = 0; i < FD_SETSIZE; i++)
    {
        watched_client_fds[i] = -1;
    }

    char buffer[1024] = {0}; // 读写缓冲区

    fd_set watched_read_fds_set, readfds; // 被监听的文件描述符集(readfds的备份)，select的in、out参数，select返回时readfds会被修改

    //     void FD_ZERO( fd_set * set );
    FD_ZERO(&watched_read_fds_set); // 清空

    //     void FD_SET( int fd, fd_set * set );
    FD_SET(server_fd, &watched_read_fds_set); // 添加服务器监听客户端连接的文件描述符

    // 循环监听服务器的套接字文件和已经建立连接的accept返回的客户端套接字文件，每次循环处理一个请求
    while (1)
    {
        readfds = watched_read_fds_set;

        // Select()函数可以帮助你同时监视许多套接字。它会告诉你哪一个套接字已经可以读取数据，
        //哪个套接字已经可以写入数据，甚至你可以知道哪个套接字出现了错误，
        // int select( int nfds, fd_set * readfds, fd_set * writefds, fd_set * exceptfds, struct timeval* timeout );
        int ready_num = select(curr_max_fd + 1, &readfds, NULL, NULL, NULL);
        if (ready_num == -1)
        {
            perror("select");
            exit(-1);
        }
        else if (ready_num == 0)
        {
            continue;
        }

        // 同时可能有多个请求到达，包括新的客户端连接请求和已经建立连接的客户端发一个新的消息
        // 但是每次调用selecet只返回一个连接请求，多个连接请求需要多次调用select来处理

        struct sockaddr_in new_clinet_addr;
        socklen_t new_clinetaddr_len;
        char client_ip[16] = {0};
        int clinet_port;

        // 是否有新的连接请求到达
        if (FD_ISSET(server_fd, &readfds)) // int FD_ISSET( int fd, fd_set * set );
        {
            // int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
            int new_clinet_fd = accept(server_fd, (struct sockaddr *)&new_clinet_addr, &new_clinetaddr_len); // 此时不会阻塞
            if (new_clinet_fd == -1)
            {
                perror("accept");
                exit(-1);
            }
            //const char *inet_ntop(int af, const void *src, char* dst, socklen_t size);
            char *p = inet_ntop(AF_INET, &new_clinet_addr.sin_addr, client_ip, 16);
            if (!p)
            {
                perror("inet_ntop");
                exit(-1);
            }
            clinet_port = ntohs(new_clinet_addr.sin_port);
            printf("Successfully received connection request from client %s:%d \n", client_ip, clinet_port);

            // 将新的客户端文件描述符添加到监视的文件描述符集合中
            FD_SET(new_clinet_fd, &watched_read_fds_set);

            for (int i = 0; i < FD_SETSIZE; i++) // 修改已监听客户端集合
            {
                if (watched_client_fds[i] < 0)
                {
                    watched_client_fds[i] = new_clinet_fd;
                    break;
                }
            }

            // 更新最大已监视文件描述符
            if (curr_max_fd < new_clinet_fd)
            {
                curr_max_fd = new_clinet_fd;
            }

            if (--ready_num == 0)
            {
                continue; // 只有一个新的连接请求，没有客户端发数据
            }
        }

        // 处理客户端发送过来的数据
        for (int i = 0; i < curr_max_fd; i++)
        {
            if (watched_client_fds[i] < 0)
            {
                continue;
            }

            if (FD_ISSET(watched_client_fds[i], &readfds))
            {
                --ready_num;

                // ssize_t read(int fd, void *buf, size_t count);
                ssize_t size = read(watched_client_fds[i], buffer, 1024);
                if (size == -1)
                {
                    perror("read");
                    exit(-1);
                }
                else if (size == 0)
                {
                    // 客户端已经关闭
                    close(watched_client_fds[i]);
                    FD_CLR(watched_client_fds[i], &watched_read_fds_set);
                    watched_client_fds[i] = -1;
                }
                else
                {
                    ret = write(STDOUT_FILENO, buffer, size); // 显示
                    if (ret == -1)
                    {
                        perror("write");
                        exit(-1);
                    }

                    // 转大写后写回客户端
                    for (int i = 0; i < strlen(buffer); i++)
                    {
                        if (isalpha(buffer[i]))
                        {
                            buffer[i] = toupper(buffer[i]);
                        }
                    }
                    ret = write(watched_client_fds[i], buffer, size);
                    if (ret == -1)
                    {
                        perror("write");
                        exit(-1);
                    }
                }
                memset(buffer, 0, 1024);
            } //if

            if (ready_num == 0)
            {
                break;
            }
        } // for
    }
}