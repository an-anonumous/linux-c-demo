#include <stdio.h>
#include <sys/types.h>         
#include <strings.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>

// #define SERVER_SOCKET_FILE_PATH ("my-local-socket-server-file")
#define SERVER_SOCKET_FILE_PATH ("server.socket")
#define BUFFER_SIZE (1024)

int main()
{
    char buffer[BUFFER_SIZE];

    // 初始化服务器端地址
    struct sockaddr_un server_addr, client_addr;
    bzero( &server_addr, sizeof( server_addr ) );
    server_addr.sun_family = AF_UNIX;
    strcpy( server_addr.sun_path, SERVER_SOCKET_FILE_PATH );

    // int socket(int domain, int type, int protocol);
    int server_fd = socket( AF_UNIX, SOCK_STREAM, 0 );
    if (server_fd < 0)
    {
        printf( "%s:%d->%s error \n", __FILE__, __LINE__, __FUNCTION__ );
        perror( "socket" );
        exit( -1 );
    }

    // 获取服务器地址的实际大小
    int len = offsetof( struct sockaddr_un, sun_path ) + strlen( server_addr.sun_path );
    unlink( SERVER_SOCKET_FILE_PATH );      // 确保当前目录下没有同名文件
    // int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    int ret = bind( server_fd, (struct sockaddr*) & server_addr, len );
    if (ret == -1)
    {
        printf( "%s:%d->%s error \n", __FILE__, __LINE__, __FUNCTION__ );
        perror( "bind" );
        exit( -1 );
    }

    // int listen(int sockfd, int backlog);
    ret = listen(server_fd,20);
    if (ret == -1)
    {
        printf( "%s:%d->%s error \n", __FILE__, __LINE__, __FUNCTION__ );
        perror( "listen" );
        exit( -1 );
    }

    while (1)
    {
        bzero( &client_addr, sizeof( client_addr ) );
        len = sizeof( client_addr );
        // int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
        int client_fd = accept( server_fd, (struct sockaddr*) & client_addr, (socklen_t*)&len );
        if (client_fd == -1)
        {
            printf( "%s:%d->%s error \n", __FILE__, __LINE__, __FUNCTION__ );
            perror( "accept" );
            exit( -1 );
        }
        len -= offsetof( struct sockaddr_un, sun_path );      /* 得到文件名的长度 */
        client_addr.sun_path[len] = '\0';
        printf( "client bind filename %s\n", client_addr.sun_path );

        int size = -1;
        while (1)
        {
            bzero( buffer, BUFFER_SIZE );
            size = read( client_fd, buffer, BUFFER_SIZE - 1 );
            if (size < 0)
            {
                printf( "%s:%d->%s error \n", __FILE__, __LINE__, __FUNCTION__ );
                perror( "read" );
                exit( -1 );
            }
            else if (size == 0)
            {
                close( client_fd );
                close( server_fd );
                exit( -1 );
            }

            for (int i = 0; i < strlen( buffer ); i++)
            {
                if (islower( buffer[i] ))
                {
                    buffer[i] = toupper( buffer[i] );
                    continue;
                }

                if (isupper( buffer[i] ))
                {
                    buffer[i] = tolower( buffer[i] );
                }
            }
            // 写会客户端
            // ssize_t write(int fd, const void *buf, size_t count);
            size = write( client_fd, buffer, strlen( buffer ) );
            if (size <= 0)
            {
                printf( "%s:%d->%s error \n", __FILE__, __LINE__, __FUNCTION__ );
                perror( "read" );
                exit( -1 );
            }
        }

    }
}