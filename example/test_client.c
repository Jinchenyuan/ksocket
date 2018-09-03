#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../ksock.h"

#define SERVER_ADDR     "192.168.44.132"
#define SERVER_PORT     6004

char send_buf[100];

int main(int argc, char const *argv[])
{
    const char *addr = SERVER_ADDR;
    uint16_t port = SERVER_PORT;
    if (3 == argc)
    {
        addr = argv[1];
        port = atoi(argv[2]);
    }
    //第一步：先socket打开文件描述符
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

    //第二步：connect连接服务器
    ret = k_connect(hd, addr, port, KSOCK_INET);
    if (ret == KSOCK_ERR)
    {
        k_perror("k_connect");
        return -1;
    }
    printf("connect success %d\n", ret);
    
    struct ksock_connect_node node;
    ret = k_get_connect_node(hd, &node);
    if (ret == KSOCK_ERR)
    {
        k_perror("k_get_connect_node");
        return -1;
    }
    char *tmp[10] = {"hello world!", "2", "3", "4", "5", "6", "7", "8", "9", "10"};
    int idx = 0;
    while (idx < 10)
    {
        strcpy(send_buf, tmp[idx]);
        ret = k_send(node, send_buf, strlen(send_buf), 0);
        if (ret < 0)
        {
            perror("k_send");
            return -1;
        }
        printf("send %d char\n", ret);
        idx++;
    }
    sleep(100);
    return 0;
}
