#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <strings.h>
#include <string.h>

#define  SERVER_IP ("10.168.57.226")
#define  SERVER_PORT (8000)

#define BUFFER_SIZE (1024)

int main()
{
    struct sockaddr_in server_addr;
    memset(&server_addr ,0,sizeof( server_addr ) );
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    
    char buffer[BUFFER_SIZE] = { 0 };
    
    // int inet_pton(int af, const char *src, void *dst);
    int ret = inet_pton( AF_INET, SERVER_IP, (struct sockaddr*) & server_addr.sin_addr.s_addr );
    if (ret != 1)
    {
        printf( "%s:%d::%s inet_pton error  \n", __FILE__, __LINE__, __FUNCTION__ );
        return -1;
    }

    int clinet_fd = socket( AF_INET, SOCK_DGRAM, 0 );
    if (clinet_fd == -1)
    {
        printf( "%s:%d::%s socket error  \n", __FILE__, __LINE__, __FUNCTION__ );
        return -1;
    }

    while (fgets( buffer, BUFFER_SIZE, stdin ) != NULL) {
       int  n = sendto( clinet_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr*) & server_addr, sizeof( server_addr ) );
        if (n == -1)
            perror( "sendto error" );

        n = recvfrom( clinet_fd, buffer, BUFFER_SIZE-1, 0, NULL, 0 );         //NULL:不关心对端信息
        if (n == -1)
            perror( "recvfrom error" );

        write( STDOUT_FILENO, buffer , BUFFER_SIZE );
    }
}
