#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <net/if.h>

#define SERVER_PORT 8000
#define CLIENT_PORT 9000

#define GROUP "239.0.0.2"

int main(int argc, char *argv[])
{
    struct sockaddr_in local_addr;
    ssize_t len;
    char buf[BUFSIZ];

    struct ip_mreqn group;                                                  /* 组播结构体 */

    int client_fd = socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&local_addr, sizeof(local_addr));                                   /* 初始化 */
    local_addr.sin_family = AF_INET;
    inet_pton(AF_INET, "0.0.0.0" , &local_addr.sin_addr.s_addr);
    local_addr.sin_port = htons(CLIENT_PORT);

    bind(client_fd, (struct sockaddr *)&local_addr, sizeof(local_addr));

    inet_pton(AF_INET, GROUP, &group.imr_multiaddr);                        /* 设置组地址 */
    inet_pton(AF_INET, "0.0.0.0", &group.imr_address);                      /* 使用本地任意IP添加到组播组 */
    group.imr_ifindex = if_nametoindex("eth0");                             /* 通过网卡名-->编号 ip ad */
    
    setsockopt(client_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &group, sizeof(group));/* 设置client 加入多播组 */

    while (1) {
        len = recvfrom(client_fd, buf, sizeof(buf), 0, NULL, 0);
        write(STDOUT_FILENO, buf, len);
    }
    close(client_fd);

    return 0;
}

