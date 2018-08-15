#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>
#include "ksock.h"

const int HD_SIZE = 79;
struct ksock_node *_hd_array[HD_SIZE] = {NULL};

int __k_add(const int fd, const struct ksock_init i)
{
    struct ksock_node *p = malloc(sizeof(struct ksock_node));
    p->init.af = i.af;
    p->init.proto = i.proto;
    p->fd = fd;
    p->state = KSOCK_STATE_SLEEP;
    p->mode = KSOCK_UNKNOW;
    
    for(int i = 0; i < HD_SIZE && (NULL != _hd_array[i]); i++)
    {
        _hd_array[i] = p;
        return i;
    }
    return KSOCK_ERR;
}

int __k_remove(const int hd)
{
    if (hd < 0 || hd >= HD_SIZE)
        return KSOCK_ERR;
    struct ksock_node *p = _hd_array[hd];
    if (NULL != p)
    {
        free(p);
        _hd_array[hd] = NULL;
    }
    else
    {
        return KSOCK_ERR;
    }
}

int k_socket(const struct ksock_init i)
{
    int sockfd = -1;
    sockfd = socket(i.af, i.proto, 0);
    if (sockfd < 0)
    {
        perror("k_socket");
        return KSOCK_ERR;
    }
    __k_add(sockfd, i);
}

/*
listen
means create a socket fd
kp:port
p_type:proto type, see ksock_proto
discr: other description about socket, use bit operating
*/
int k_listen(const k_port kp, const short p_type, const int b_discr)
{
    //第一步：先socket打开文件描述符
    int sockfd = -1;
    struct sockaddr_in server_addr = {0};
    struct sockaddr_in client_addr = {0};
    int ret = -1;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sockfd)
    {
        perror("socket");
        return -1;
    }
    printf("sockfd = %d\n", sockfd);

    //第二步：bind绑定sockfd和当前电脑的ip地址和端口号
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    ret = bind(sockfd, (const struct sockaddr*)&server_addr, sizeof(server_addr));
    if (ret < 0)
    {
        perror("bind");
        return -1;
    }
    printf("bind success\n");

    //第三步：listen监听端口
    ret = listen(sockfd, LISTEN_NUM);
    if (ret < 0)
    {
        perror("listen");
        return -1;
    }

    return KSOCK_SUC;
}