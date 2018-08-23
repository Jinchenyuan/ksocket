#include <stdio.h>
#include "../ksock.h"

#define SERVER_ADDR     "192.168.44.132"
#define SERVER_PORT     6004

char send_buf[100];

int main(int argc, char const *argv[])
{
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
    ret = k_connect(hd, SERVER_ADDR, SERVER_PORT, KSOCK_INET);
    if (ret == KSOCK_ERR)
    {
        k_perror("k_connect");
        return -1;
    }
    printf("connect success %d\n", ret);
    
    strcpy(send_buf, "hello world.");
    struct kscok_connect_node *node = {0};
    ret = k_get_connect_node(hd, node);
    if (ret == KSOCK_ERR)
    {
        k_perror("k_get_connect_node");
        return -1;
    }
    k_send(node->fd, send_buf, strlen(send_buf), 0);
    if (ret == KSOCK_ERR)
    {
        perror("send");
        return -1;
    }
    printf("send %d char\n", ret);
    return 0;
}
