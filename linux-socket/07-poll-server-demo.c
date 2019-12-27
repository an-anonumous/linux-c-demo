#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>
#include <sys/select.h>

#define SERVER_IP ("10.168.57.226")
#define SERVER_POT (9988)

int main()
{
    // 创建socket文件
    // int socket(int domain, int type, int protocol);
    int server_fd = socket( AF_INET, SOCK_STREAM, 0 );
    if (server_fd == -1)
    {
        perror( "socket" );
        exit( -1 );
    }

    // 允许端口复用
    int opt = 1, ret;
    setsockopt( server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof( opt ) );

    // 初始化服务器本地地址
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons( SERVER_POT );

    // int inet_pton(int af, const char *src, void *dst);
    ret = inet_pton( AF_INET, SERVER_IP, &server_addr.sin_addr.s_addr );
    if (ret != 1)
    {
        perror( "inet_pton" );
        exit( -1 );
    }

    // 本地socket文件绑定地址
    // int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    ret = bind( server_fd, (struct sockaddr*) & server_addr, sizeof( server_addr ) );
    if (ret != 0)
    {
        perror( "bind" );
        exit( -1 );
    }

    // 开始监听
    // int listen(int sockfd, int backlog);
    ret = listen( server_fd, 128 );
    if (ret != 0)
    {
        perror( "listen" );
        exit( -1 );
    }
    char buffer[1024] = { 0 }; // 读写缓冲区

    struct pollfd fds_arr[FD_SETSIZE];
    bzero( fds_arr, sizeof( fds_arr ) ); // void bzero(void *s, size_t n);

    // 监听服务器的套接字文件
    fds_arr[0].fd = server_fd;
    fds_arr[0].events = POLLIN;
    fds_arr[0].revents = 0;

    int fds_arr_len = 1;

    // 循环监听服务器的套接字文件和已经建立连接的accept返回的客户端套接字文件，每次循环处理一个请求
    while (1)
    {
        // int poll(struct pollfd *fds, nfds_t nfds, int timeout);
        int ready_num = poll( fds_arr, fds_arr_len, -1 );
        if (ready_num == -1)
        {
            perror( "poll" );
            exit( -1 );
        }
        else if (ready_num == 0)
        {
            continue;
        }

        struct sockaddr_in new_clinet_addr;
        socklen_t new_clinetaddr_len;
        char client_ip[16] = { 0 };
        int clinet_port;

        // 先检查是否有新的连接请求到达
        if (fds_arr[0].revents & POLLIN)
        {
            // int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
            int new_clinet_fd = accept( server_fd, (struct sockaddr*) & new_clinet_addr, &new_clinetaddr_len ); // 此时不会阻塞
            if (new_clinet_fd == -1)
            {
                perror( "accept" );
                exit( -1 );
            }

            // 将新连接的客户端socket文件描述符添加到poll监视的文件描述符集合中
            fds_arr[fds_arr_len].fd = new_clinet_fd;
            fds_arr[fds_arr_len].events = POLLIN;
            fds_arr[fds_arr_len].revents = 0;
            fds_arr_len++;

            //const char *inet_ntop(int af, const void *src, char* dst, socklen_t size);
            char* p = inet_ntop( AF_INET, &new_clinet_addr.sin_addr, client_ip, 16 );
            if (!p)
            {
                perror( "inet_ntop" );
                exit( -1 );
            }
            clinet_port = ntohs( new_clinet_addr.sin_port );
            printf( "Successfully received connection request from client %s:%d \n", client_ip, clinet_port );

            if (--ready_num == 0)
            {
                continue; // 只有一个新的连接请求，没有客户端发数据
            }
        }

        // 处理客户端发送过来的数据
        for (int i = 1; i < fds_arr_len; i++)
        {
            if (fds_arr[i].fd >= 0 && (fds_arr[i].revents & POLLIN))
            {
                --ready_num;

                // ssize_t read(int fd, void *buf, size_t count);
                ssize_t size = read( fds_arr[i].fd, buffer, 1024 );
                if (size == -1)
                {
                    perror( "read" );
                    exit( -1 );
                }
                else if (size == 0)
                {
                    // 客户端已经关闭
                    close( fds_arr[i].fd );
                    fds_arr[i].fd = -1;
                }
                else
                {
                    ret = write( STDOUT_FILENO, buffer, size ); // 显示
                    if (ret == -1)
                    {
                        perror( "write" );
                        exit( -1 );
                    }

                    // 转大写后写回客户端
                    for (int i = 0; i < strlen( buffer ); i++)
                    {
                        if (isalpha( buffer[i] ))
                        {
                            buffer[i] = toupper( buffer[i] );
                        }
                    }
                    ret = write( fds_arr[i].fd, buffer, size );
                    if (ret == -1)
                    {
                        perror( "write" );
                        exit( -1 );
                    }
                }
                memset( buffer, 0, 1024 );
            } //if

            if (ready_num == 0)
            {
                break;
            }
        } // for
    }
}