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
// #define CLIENT_SOXKET_FILE_PATH ("my-local-socket-client-file")
#define SERVER_SOCKET_FILE_PATH ("server.socket")
#define CLIENT_SOXKET_FILE_PATH ("client.socket")
#define BUFFER_SIZE (1024)

int main()
{
    struct sockaddr_un server_addr, client_addr;

    // 初始化服务器端地址
    bzero( &server_addr, sizeof( server_addr ) );
    server_addr.sun_family = AF_UNIX;
    strcpy( server_addr.sun_path, SERVER_SOCKET_FILE_PATH );

    // 初始化客户端本地地址
    bzero( &client_addr, sizeof( client_addr ) );
    client_addr.sun_family = AF_UNIX;
    strcpy( client_addr.sun_path, CLIENT_SOXKET_FILE_PATH );

    char buffer[BUFFER_SIZE] = { 0 };

    // int socket(int domain, int type, int protocol);
    int client_fd = socket( AF_UNIX, SOCK_STREAM, 0 );
    if (client_fd < 0)
    {
        printf( "%s:%d->%s error \n", __FILE__, __LINE__, __FUNCTION__ );
        perror( "socket" );
        exit( -1 );
    }

    int len = offsetof( struct sockaddr_un, sun_path ) + strlen( client_addr.sun_path );     /* 计算客户端地址结构有效长度 */
    unlink( CLIENT_SOXKET_FILE_PATH );
    // int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    int ret = bind( client_fd, (struct sockaddr*) & client_addr, len );
    if (ret == -1)
    {
        printf( "%s:%d->%s error \n", __FILE__, __LINE__, __FUNCTION__ );
        perror( "bind" );
        exit( -1 );
    }

    len = offsetof( struct sockaddr_un, sun_path ) + strlen( server_addr.sun_path );   /* 计算服务器端地址结构有效长度 */
    // int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    ret = connect(client_fd,(struct sockaddr *)&server_addr,len);
    if (ret != 0)
    {
        printf( "%s:%d->%s error \n", __FILE__, __LINE__, __FUNCTION__ );
        perror( "bind" );
        exit( -1 );
    }

    while (fgets( buffer, sizeof( buffer ), stdin ) != NULL) {
        write( client_fd, buffer, strlen( buffer ) );
        len = read( client_fd, buffer, sizeof( buffer ) );
        write( STDOUT_FILENO, buffer, len );
    }

    close( client_fd );

    return 0;
}