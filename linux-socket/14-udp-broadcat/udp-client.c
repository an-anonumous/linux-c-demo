#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <strings.h>
#include <string.h>

// #define  SERVER_IP ("10.168.57.226")
// #define  SERVER_PORT (8000)

#define LOCAL_PORT (9898)

#define BUFFER_SIZE (1024)

int main()
{
    // 初始化本地地址、端口
    struct sockaddr_in client_addr;
    memset(&client_addr ,0,sizeof( client_addr ) );
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons( LOCAL_PORT );
    client_addr.sin_addr.s_addr = htonl( INADDR_ANY );

    char buffer[BUFFER_SIZE] = { 0 };

    int clinet_fd = socket( AF_INET, SOCK_DGRAM, 0 );
    if (clinet_fd == -1)
    {
        printf( "%s:%d::%s socket error  \n", __FILE__, __LINE__, __FUNCTION__ );
        return -1;
    }

    // int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    int ret = bind(clinet_fd,(struct sockaddr*)&client_addr,sizeof(client_addr));
    if (ret == -1)
    {
        printf( "%s:%d::%s bind error  \n", __FILE__, __LINE__, __FUNCTION__ );
    }

    while (1) {
        int size = recvfrom( clinet_fd, buffer, BUFFER_SIZE-1, 0, NULL, 0 );         //NULL:不关心对端信息
        if (size == -1)
        {
            perror( "recvfrom error" );
        }

        write( STDOUT_FILENO, buffer , size );
    }
}
