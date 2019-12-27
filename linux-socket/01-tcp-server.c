#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{

    //    1. socket()  建立服务器本地的套接字
    //int socket(int domain, int type, int protocol);
    int server_fd = socket( AF_INET, SOCK_STREAM, 0);    // tcp
    if (server_fd == -1)
    {
        perror("socket");
        exit( 1 );
    }

    // 允许端口复用
	int opt = 1;
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    //    2. bind() 绑定本地IP 端口号( struct sockaddr_in addr 初始化 )
    struct sockaddr_in server_addr, client_addr;
    char server_ip[] = "10.168.57.226";

    // int inet_pton( int af, const char* src, void* dst );
    int ret = inet_pton(AF_INET, server_ip, &server_addr.sin_addr.s_addr); // 将字符串的地址转为32位整数，再转为网络字节序（小端转大端）
    if (ret != 1)
    {
        perror( "inet_pton" );
        exit( -1 );
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons( 6666 );  // The htons() function converts the unsigned short integer hostshort from host byte order to network byte order.

    // int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    ret = bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret != 0)
    {
        perror("bind");
        exit( -1 );
    }

    //    3. listen() 指定最大同时发起连接数
    // int listen( int sockfd, int backlog );
    ret = listen(server_fd, 10);
    if (ret != 0)
    {
        perror( "listen" );
        exit( -1 );
    }

    //    4. accept()  阻塞等待客户端发起连接
    
    socklen_t clinet_addr_len = sizeof(client_addr);

    // int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &clinet_addr_len );
    if (ret == -1)
    {
        perror( "accept" );
        exit( -1 );
    }

    char client_ip_str[13] = { 0 };
    // const char* inet_ntop( int af, const void* src, char* dst, socklen_t size );
    char * p = inet_ntop(AF_INET, &client_addr.sin_addr.s_addr, client_ip_str, 13); // 将客户端的地址从网络字节序(大端)的整数转换为字符串
    if (p == NULL)
    {
        perror("inet_ntop");
        exit( -1 );
    }

    //    5. read()
    char buffer[1024] = { 0 };
    while (1)
    {
        // ssize_t read(int fd, void *buf, size_t count);
        ssize_t r_size = read(client_fd, buffer, 1024); // 读取客户端的消息
        if (r_size == -1)
        {
            perror( "read" );
            exit( -1 );
        }

        printf("received from %s : %d %s \n ", client_ip_str, ntohs(client_addr.sin_port), buffer);

        //    7. write 给 客户端
        write(client_fd, "received\n", strlen( "received\n" ));
        
        // void *memset(void *s, int c, size_t n);
        memset(buffer,0,sizeof(buffer));
    }

    //    8. close();
    close(client_fd);
    close( server_fd );
}
