#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <strings.h>

#define  SERVER_IP ("10.168.57.226")
#define  SERVER_PORT (8000)

#define BUFFER_SIZE (1024)

int main()
{
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    // int inet_pton(int af, const char *src, void *dst);
    int ret = inet_pton(AF_INET, SERVER_IP,(struct sockaddr *)&server_addr.sin_addr.s_addr);
    if (ret != 1)
    {
        printf( "%s:%d::%s inet_pton error  \n", __FILE__, __LINE__, __FUNCTION__ );
        return -1;
    }

    int server_fd = socket(AF_INET,SOCK_DGRAM,0);
    if (server_fd == -1)
    {
        printf( "%s:%d::%s socket error  \n", __FILE__, __LINE__, __FUNCTION__ );
        return -1;
    }

    // int bind( int sockfd, const struct sockaddr* addr, socklen_t addrlen );
    ret = bind(server_fd,(struct sockaddr *)&server_addr,sizeof(server_addr));
    if (ret != 0)
    {
        printf( "%s:%d::%s bind error  \n", __FILE__, __LINE__, __FUNCTION__ );
        perror("bind");
        return -1;
    }

    char buffer[1024] = { 0 };

    struct sockaddr_in client_addr;
    memset(&client_addr ,0,sizeof(client_addr));
    socklen_t client_addr_len;
    char client_ip[16] = { 0 };
    int clinet_port;
    while (1)
    {
        client_addr_len = sizeof( client_addr );
        // ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr /*out*/, socklen_t *addrlen /*in,out*/);
        ssize_t size = recvfrom(server_fd,buffer, BUFFER_SIZE-1 ,0,(struct sockaddr *)&client_addr,&client_addr_len);
        if (size == -1)
        {
            printf( "%s:%d::%s recvfrom error  \n", __FILE__, __LINE__, __FUNCTION__ );
        }

        // const char *inet_ntop(int af, const void *src, char *dst, socklen_t size);
        char * p = inet_ntop(AF_INET,&client_addr.sin_addr.s_addr,client_ip,16);
        if (!p)
        {
            printf( "%s:%d::%s inet_ntop error  \n", __FILE__, __LINE__, __FUNCTION__ );
        }
        clinet_port = ntohs(client_addr.sin_port);

        printf("receive form %s:%d \t %s \n",client_ip,clinet_port,buffer);

        for (int i=0;i<strlen(buffer);i++)
        {
            if (islower(buffer[i]))
            {
                buffer[i] = toupper( buffer[i] );
            }
            else if (isupper( buffer[i] ))
            {
                buffer[i] = tolower( buffer[i] );
            }
        }

        // Ð´»á¿Í»§¶Ë
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