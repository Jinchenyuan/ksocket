#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>
#include "ksock.h"

const int HD_SIZE = 79;
const int LISTEN_QUEUE_MAX_NUM = 1000;
const int ACCEPT_QUEUE_MAX_NUM = 1000;

struct ksock_node *_hd_array[HD_SIZE] = {NULL};

int __k_add(const int fd, const struct ksock_init i)
{
    struct ksock_node *p = malloc(sizeof(struct ksock_node));
    p->init.af = i.af;
    p->init.proto = i.proto;
    p->fd = fd;
    p->state = KSOCK_STATE_SLEEP;
    p->accpet_head = NULL;
    p->accpet_tail = NULL;
    p->accept_thread = NULL;
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
        if(NULL != p->accept_thread)
        {
            int i;
        }
        close(p->fd);
        free(p);
        _hd_array[hd] = NULL;
        return KSOCK_SUC;
    }
    else
    {
        return KSOCK_ERR;
    }
}

void *accept_func(const int hd)
{

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

int k_listen(const int hd, const char *address, const uint16_t port, const short family)
{
    if (NULL == _hd_array[hd])
        return KSOCK_ERR;

    struct sockaddr_in addr_in = {0};
    int ret = -1;

    addr_in.sin_family = family;
    addr_in.sin_port = htons(port);
    addr_in.sin_addr.s_addr = inet_addr(address);
    ret = bind(_hd_array[hd]->fd, (const struct sockaddr*)&addr_in, sizeof(addr_in));
    if (ret < 0)
    {
        perror("k_listen of bind");
        return KSOCK_ERR;
    }

    ret = listen(_hd_array[hd]->fd, LISTEN_QUEUE_MAX_NUM);
    if (ret < 0)
    {
        perror("k_listen");
        return KSOCK_ERR;
    }
    _hd_array[hd]->mode = KSOCK_SERVER;
    _hd_array[hd]->state = KSOCK_STATE_LISTEN;
    return KSOCK_SUC;
}

int k_accept(const int hd)
{
    if (NULL == _hd_array[hd])
        return KSOCK_ERR;

    if (NULL != _hd_array[hd]->accept_thread)
    {
        //后续要输出日志，表示正确的特殊性
        return KSOCK_SUC;
    }

    pthread_create(&(_hd_array[hd]->accept_thread), NULL, accept_func, hd);
    return KSOCK_SUC;
}