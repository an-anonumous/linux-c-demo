#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

// client

int main()
{
    int ret ;
    char* txt[] = 
    { 
        "听我誓言，作吾见证：",
        "Hear my words, and bear witness to my vow.",
        "长夜将至，我从今开始守望，至死方休。",
        "Night gathers, and now my watch begins.",
        "我将不娶妻，不封地，不生子。",
        "I shall take no wife, hold no lands, father no children.",
        "我将不戴王冠，不争荣宠。",
        "I shall wear no crowns and win no glory.",
        "我将尽忠职守，生死于斯。",
        "I shall live and die at my post.",
        "我是黑暗中的利剑，",
        "I am the sword in the darkness,",
        "长城上的守卫，",
        "I am the watcher on the walls.",
        "抵御寒冷的烈焰，",
        "I am the fire that burns against the cold,",
        "破晓时分的光线，",
        "the light that brings the dawn,",
        "唤醒眠者的号角，",
        "the horn that wakes the sleepers,",
        "守护王国的坚盾。",
        "the shield that guards the realms of men.",
        "我将生命与荣耀献给守夜人，",
        "I pledge my life and honor to the Night's Watch,",
        "今夜如此，夜夜皆然。",
        "for this night, and all the nights to come." 
    };

    // 建立客户端本地socket
    // int socket(int domain, int type, int protocol);
    int local_fd = socket(AF_INET, SOCK_STREAM ,0);
    if (local_fd == -1)
    {
        perror("socket");
        exit( -1 );
    }

    // 绑定本地ip +端口 
    struct sockaddr_in local_addr , server_addr;
   /* local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(3333);

    char local_ip[] = "192.168.119.134";
    // int inet_pton( int af, const char* src, void* dst );
    int ret = inet_pton( AF_INET, local_ip, &local_addr.sin_addr.s_addr ); // 将字符串的地址转为32位整数，再转为网络字节序（小端转大端）
    if (ret != 1)
    {
        perror( "inet_pton" );
        exit( -1 );
    }

    // int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    ret = bind(local_fd,(struct sockaddr *)&local_addr,sizeof(local_addr));     // 如果省略这一步，os会自动分配一个地址和端口
    if (ret != 0)
    {
        perror( "bind" );
        exit( -1 );
    }
*/

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(6666);

    char server_ip[] = "10.168.57.226";
    // int inet_pton( int af, const char* src, void* dst );
    ret = inet_pton( AF_INET, server_ip, &server_addr.sin_addr.s_addr ); // 将字符串的地址转为32位整数，再转为网络字节序（小端转大端）
    if (ret != 1)
    {
        perror( "inet_pton" );
        exit( -1 );
    }

    // int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    ret = connect(local_fd,(struct sockaddr *)&server_addr,sizeof(server_addr));
    if (ret != 0)
    {
        perror( "connect" );
        exit( -1 );
    }

    char buf[1024] = { 0 };

    // 循环发送
    int i=0;
    for(;i< sizeof(txt)/sizeof(char *);i++)
    {
        write( local_fd, txt[i], strlen( txt[i] ) );  
                
        /*从服务器读回转换后数据*/
       int len = read( local_fd, buf, sizeof( buf ) );

        /*写至标准输出*/
        write( STDOUT_FILENO, buf, len );
    }

    // 等待从键盘输入
    while (1) {
        /*从标准输入获取数据*/
        fgets( buf, sizeof( buf ), stdin );

        /*将数据写给服务器*/
        write( local_fd, buf, strlen( buf ) );       //写服务器

        /*从服务器读回转换后数据*/
       int len = read( local_fd, buf, sizeof( buf ) );

        /*写至标准输出*/
        write( STDOUT_FILENO, buf, len );

        //void *memset(void *s, int c, size_t n);
        memset(buf,0,1024);
    }

    close(local_fd);
}
