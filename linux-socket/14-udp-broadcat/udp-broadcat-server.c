#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <strings.h>
#include <net/if.h>

//#define  SERVER_IP ("10.168.57.226")
#define  SERVER_PORT (8000)

//#define BROADCAST_IP ("10.168.34.255")
#define BROADCAST_IP ("192.168.199.255")
#define CLIENT_PORT  (9898)

#define BUFFER_SIZE (1024)

int main()
{
    struct sockaddr_in server_addr;
    memset( &server_addr,0,sizeof(server_addr) );
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int server_fd = socket(AF_INET,SOCK_DGRAM,0);
    if (server_fd == -1)
    {
        printf( "%s:%d::%s socket error  \n", __FILE__, __LINE__, __FUNCTION__ );
        return -1;
    }

    // int bind( int sockfd, const struct sockaddr* addr, socklen_t addrlen );
    int ret = bind(server_fd,(struct sockaddr *)&server_addr,sizeof(server_addr));
    if (ret != 0)
    {
        printf( "%s:%d::%s bind error  \n", __FILE__, __LINE__, __FUNCTION__ );
        perror("bind");
        return -1;
    }

    // 允许服务器端广播
    int flag = 1;
    setsockopt( server_fd, SOL_SOCKET, SO_BROADCAST, &flag, sizeof( flag ) );

    char buffer[1024] = { 0 };

    // 客户端地址
    struct sockaddr_in client_addr;
    memset(&client_addr ,0,sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(CLIENT_PORT);
    const char * p = inet_pton( AF_INET,BROADCAST_IP,&client_addr.sin_addr.s_addr);
    if (!p)
    {
        printf( "%s:%d::%s inet_ntop error  \n", __FILE__, __LINE__, __FUNCTION__ );
    }

    int i = 0;
    while (1)
    {
        sleep(1);
        sprintf( buffer, "hello %d \n", i++ );
        puts(buffer);

        // 写回客户端
        // ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);
        ret = sendto(server_fd,buffer,BUFFER_SIZE-1,0,(struct sockaddr *)&client_addr, sizeof( client_addr ) );
        if (ret == -1)
        {
            printf( "%s:%d::%s sendto error  \n", __FILE__, __LINE__, __FUNCTION__ );
            return -1;
        }

        bzero(buffer,BUFFER_SIZE);
    }
    close(server_fd);
    return 0;
}