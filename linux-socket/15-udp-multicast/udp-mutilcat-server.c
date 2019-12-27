#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <net/if.h>

#define SERVER_PORT 8000
#define CLIENT_PORT 9000

#define GROUP "239.0.0.2"

#define BUFFER_SIZE 1500

int main(void)
{
    char buf[BUFFER_SIZE] = "hello udp mutilcast \n";

    struct ip_mreqn group;
    struct sockaddr_in server_addr, client_addr;

    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;                /* IPv4 */
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); /* 本地任意IP INADDR_ANY = 0 */
    server_addr.sin_port = htons(SERVER_PORT);

    int server_fd = socket(AF_INET, SOCK_DGRAM, 0); /* 构造用于UDP通信的套接字 */

    bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    inet_pton(AF_INET, GROUP, &group.imr_multiaddr);   /* 设置组地址 */
    inet_pton(AF_INET, "0.0.0.0", &group.imr_address); /* 本地任意IP */
    group.imr_ifindex = if_nametoindex("eth0");        /* 给出网卡名,转换为对应编号: eth0 --> 编号  命令:ip ad */

    setsockopt(server_fd, IPPROTO_IP, IP_MULTICAST_IF, &group, sizeof(group)); /* 组播权限 */

    bzero(&client_addr, sizeof(client_addr)); /* 构造 client 地址 IP+端口 */
    client_addr.sin_family = AF_INET;
    inet_pton(AF_INET, GROUP, &client_addr.sin_addr.s_addr); /* IPv4  239.0.0.2+9000 */
    client_addr.sin_port = htons(CLIENT_PORT);

    int i = 0;
    while (1)
    {
        sprintf(buf, "hello udp mutilcast %d\n", i++);
        //fgets(buf, sizeof(buf), stdin);
        sendto(server_fd, buf, strlen(buf), 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
        sleep(1);
    }

    close(server_fd);

    return 0;
}
