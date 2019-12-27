/*
   1. 在 ET 边沿触发模式下，epoll函数在每次写端写入数据，也就是新数据到达时才返回，
   如果每次发送量大于接受量，且epoll函数每次返回只读取一次，则接收端数据挤压,最终写端阻塞而死锁；
   在边沿触发模式下，每次epoll函数返回，多次调用read函数，直到写端写的数据全部读取完成，再去调用epoll函数，
   则不会导致读端数据积压，但是如果在并发环境下一个客户端写入了大量的数据，可能导致其他客户端饥饿。
   2. 在水平出发(默认模式)下，只要有未读完的数据，epoll函数就返回，接收端数据不会挤压；
   */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <string.h>
#include <fcntl.h>
#include <strings.h>

//每次发的多，读的少
//#define SEND_SIZE (64)
//#define RECEIVE_SIZE (6)

//每次发的少，读的多
#define SEND_SIZE (5)
#define RECEIVE_SIZE (1024 * 4)

int main()
{
    int pipefds[2], ret;

    //    int pipe(int pipefd[2]);
    ret = pipe( pipefds );
    if (ret == -1)
    {
        perror( "pipe" );
        exit( -1 );
    }

    pid_t pid = fork();
    if (pid < 0)
    {
        perror( "fork" );
        exit( 1 );
    }
    else if (pid == 0)
    {
        sleep( 1 );  // 让父进程先写数据；

        // 子进程，关闭写端
        close( pipefds[1] );

        char rbuffer[RECEIVE_SIZE] = { 0 };

        //  int epoll_create(int size);
        int epfd = epoll_create( 10 );
        if (epfd == -1)
        {
            perror( "epoll_create" );
            exit( 1 );
        }

        struct epoll_event event;
        event.data.fd = pipefds[0];
        event.events = EPOLLIN | EPOLLET; //  ET 边沿触发
        //event.events = EPOLLIN;                                      //  LT 水平触发 (默认)
        // int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
        ret = epoll_ctl( epfd, EPOLL_CTL_ADD, pipefds[0], &event );
        if (ret != 0)
        {
            perror( "epoll_ctl" );
            exit( -1 );
        }

        struct epoll_event events[5];
        while (1)
        {


            printf("begin epoll_wait %s:%d\n", __FILE__, __LINE__);
            // int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
            int ready_num = epoll_wait( epfd, events, 5, -1 );
            if (ready_num < 0)
            {
                perror( "epoll_wait" );
                exit( -1 );
            }
            else if (ready_num == 0)
            {
                continue;
            }

            while (1)
            {

                printf("begin reading %s:%d\n", __FILE__, __LINE__);
                // ssize_t read(int fd, void *buf, size_t count);
                ret = read( pipefds[0], rbuffer, sizeof( rbuffer ) - 1 );
                if (ret < 0)
                {
                    perror( "read" );
                    exit( -1 );
                }
                else if (ret == 0)
                {
                    break;
                }

                printf( "read : %s \t read return value is %d \n", rbuffer, ret );
                memset( rbuffer, 0, sizeof( rbuffer ) );
                ret = -1;
            }
            usleep( 200 );
        }
    }
    else
    {
        // 父进程,关闭读端
        close( pipefds[0] );

        char ch = 'a';
        char buffer[SEND_SIZE] = { 0 };
        while (1)
        {
            for (int i = 0; i < SEND_SIZE - 1; i++)
            {
                // int fprintf(FILE *stream, const char *format, ...);
                ret = sprintf( buffer + i, "%c", ch );
                if (ret < 0)
                {
                    printf( "%s:%d sprintf error.\n", __FILE__, __LINE__ );
                    exit( -1 );
                }
            }
            ch = (ch + 1 - 'a') % ('z' - 'a') + 'a';

            //  ssize_t write(int fd, const void *buf, size_t count);
            ret = write( pipefds[1], buffer, strlen( buffer ) );
            if (ret < 0)
            {
                perror( "write" );
                exit( -1 );
            }
            printf( "send : %s \n", buffer );

            bzero( buffer, sizeof( buffer ) );
            sleep( 10 );
        }
    }
}
