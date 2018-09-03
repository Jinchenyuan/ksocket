#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../ksock.h"

#define SERVER_PORT     6004
#define SERVER_ADDR     "192.168.44.132"

char recv_buf[100];

int main(int argc, char const *argv[])
{
    //第一步：先socket打开文件描述符
    const char *addr = SERVER_ADDR;
    uint16_t port = SERVER_PORT;
    if (3 == argc)
    {
        addr = argv[1];
        port = atoi(argv[2]);
    }
    int hd = -1;
    int ret = -1;
    struct ksock_init i;
    i.af = KSOCK_INET;
    i.proto = KSOCK_TCP;
    hd = k_socket(i);
    if (KSOCK_ERR == hd)
    {
        k_perror("k_socket");
        return -1;
    }
    printf("sock hd = %d\n", hd);

    //第二步：bind绑定sockfd和当前电脑的ip地址和端口号
    ret = k_listen(hd, addr, port, KSOCK_INET);
    if (KSOCK_ERR == ret)
    {
        k_perror("k_listen");
        return -1;
    }
    ret = k_accept(hd);
    if (KSOCK_ERR == ret)
    {
        k_perror("k_accept");
        return -1;
    }

    // struct ksock_connect_node node;
    // node.fd = -1;
    long nd;
    ret = -1;
    while(ret == -1)
    {
        printf("1111111111\n");
        ret = k_get_accept_node(hd, &nd);
        printf("nd:%ld\n", nd);
        printf("2222222222\n");
        printf("ret = %d\n", ret);
        if (ret == KSOCK_SUC)
        {   
            printf("get accept!!!!!!\n");
            //收到连接，可以做想做的事情了
            ret = k_recv(nd, 100, 0);
            if (ret == KSOCK_SUC)
            {
                printf("recv success.\n");
            }
        }
        sleep(1);
    }
    // printf("qqqqqqqqqqqq\n");
    //接下来，可以去获取消息
    while(1)
    {
        struct ksock_msg msg;
        char buf[1024];
        msg.buf = (void *)buf;
        ret = k_get_recv_msg(nd, &msg);
        if (ret == KSOCK_SUC)
        {
            printf("msg: [%s]\n", (char *)msg.buf);
        }
        else
        {
            k_perror("k_get_recv_msg");
           
            ret = k_get_accept_node(hd, &nd);
            // printf("nd:%ld\n", nd);
            // printf("2222222222\n");
            // printf("ret = %d\n", ret);
            if (ret == KSOCK_SUC)
            {   
                printf("get accept!!!!!!\n");
                //收到连接，可以做想做的事情了
                ret = k_recv(nd, 100, 0);
                if (ret == KSOCK_SUC)
                {
                    printf("recv success.\n");
                }
            }
           
        }
        sleep(1);
    }

    return 0;
}
