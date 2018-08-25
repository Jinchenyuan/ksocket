#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>
#include "ksock.h"

#define HD_SIZE 79
const int LISTEN_QUEUE_MAX_NUM = 1000;
const int ACCEPT_QUEUE_MAX_NUM = 100;
struct ksock_node *_hd_array[HD_SIZE] = {NULL};

inline int __check_hd(const int hd)
{
    if (hd < 0 || hd >= HD_SIZE)
    {
        _error_msg = "hd error!";
        return KSOCK_ERR;
    }
    if (NULL == _hd_array[hd])
    {
        _error_msg = "hd not find!";
        return KSOCK_ERR;
    }
    return KSOCK_SUC;
}

void k_perror(const char *msg)
{
    char c[50] = "";
    strcat(c, msg);
    strcat(c, ": %s");
    printf(c, _error_msg);
}

int __k_add(const int fd, const struct ksock_init i)
{
    struct ksock_node *p = (struct ksock_node*)malloc(sizeof(struct ksock_node));
    p->init.af = i.af;
    p->init.proto = i.proto;
    p->fd = fd;
    p->state = KSOCK_STATE_SLEEP;
    p->accept_head = NULL;
    p->accept_tail = NULL;
    p->connect_node = NULL;
    p->accept_thread = NULL;
    p->mode = KSOCK_UNKNOW;

    for(int i = 0; i < HD_SIZE && (NULL != _hd_array[i]); i++)
    {
        _hd_array[i] = p;
        return i;
    }
    _error_msg = "no more space!";
    return KSOCK_ERR;
}

int __k_remove(const int hd)
{
    if (KSOCK_ERR == __check_hd(hd))
    {
        return KSOCK_ERR;
    }
    struct ksock_node *p = _hd_array[hd];
    
    if(NULL != p->accept_thread)
    {
        pthread_cancel(p->accept_thread);
        p->accept_thread = NULL;
    }
    k_accept_cancel(hd, 1);
    if (NULL != p->connect_node)
    {
        free(p->connect_node);
        p->connect_node = NULL;
    }
    close(p->fd);
    free(p);
    _hd_array[hd] = NULL;
    return KSOCK_SUC;
    
}

int __k_accept_push(struct ksock_accept_node *node)
{
    int hd = node->hd;
    if(_hd_array[hd]->accept_count < ACCEPT_QUEUE_MAX_NUM)
    {
        if (NULL == _hd_array[hd]->accept_tail)
        {
            _hd_array[hd]->accept_head = _hd_array[hd]->accept_head = node;
        }
        else
        {
            _hd_array[hd]->accept_tail->next = node;
            _hd_array[hd]->accept_tail = node;
        }
        _hd_array[hd]->accept_count += 1;
        return KSOCK_SUC;
    }
    else
    {
        // TODO 把错误信息返回给连接，然后断开连接
        char send_buf[100];
        strcpy(send_buf, "accept overflow.");
        send(node->fd, send_buf, strlen(send_buf), 0);
        close(node->fd);

        _hd_array[hd]->state = KSOCK_ACCEPT_OVERFLOW;
        return KSOCK_ERR;
    }
}

int __k_accept_pop(const int hd, struct ksock_accept_node *node)
{
    if (KSOCK_ERR == __check_hd(hd))
    {
        return KSOCK_ERR;
    }

    if (NULL == _hd_array[hd]->accept_head)
    {
        _error_msg = "no accept!";
        return KSOCK_ERR;
    }
    node->fd = _hd_array[hd]->accept_head->fd;
    node->hd = _hd_array[hd]->accept_head->hd;
    node->addr_in = _hd_array[hd]->accept_head->addr_in;
    node->next = NULL;
    struct ksock_accept_node *temp = _hd_array[hd]->accept_head;
    _hd_array[hd]->accept_head = _hd_array[hd]->accept_head->next;
    _hd_array[hd]->accept_count -= 1;
    free(temp);
    temp = NULL;
    if (NULL == _hd_array[hd]->accept_head)
    {
        _hd_array[hd]->accept_tail = NULL;
    }
    return KSOCK_SUC;
}

void *accept_func(void *arg)
{
    int hd = *(int*)arg;
    if (NULL == _hd_array[hd])
    {
        return NULL;
    }
    while(1)
    {
        int accept_fd = -1;
        struct sockaddr_in client_addr = {0};
        socklen_t len = 0;
        accept_fd = accept(_hd_array[hd]->fd, (struct sockaddr*)&client_addr, &len);
        struct ksock_accept_node *p = (struct ksock_accept_node *)malloc(sizeof(struct ksock_accept_node));
        p->fd = accept_fd;
        p->hd = hd;
        p->addr_in = client_addr;
        __k_accept_push(p);
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

int k_listen(const int hd, const char *address, const uint16_t port, const short family)
{
    if (KSOCK_ERR == __check_hd(hd))
    {
        return KSOCK_ERR;
    }

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
    if (KSOCK_ERR == __check_hd(hd))
    {
        return KSOCK_ERR;
    }
    
    if (NULL != _hd_array[hd]->accept_thread)
    {
        _error_msg = "this socket hd had accept!";
        return KSOCK_ERR;
    }

    pthread_create(&(_hd_array[hd]->accept_thread), NULL, accept_func, ((void *)&hd));
    return KSOCK_SUC;
}

int k_accept_cancel(const int hd, int is_clear_accept)
{
    if (KSOCK_ERR == __check_hd(hd))
    {
        return KSOCK_ERR;
    }
    
    if (NULL == _hd_array[hd]->accept_thread)
    {
        _error_msg = "accept_thread not exist!";
        return KSOCK_ERR;
    }
    pthread_cancel(_hd_array[hd]->accept_thread);
    _hd_array[hd]->accept_thread = NULL;
    _hd_array[hd]->state = KSOCK_STATE_LISTEN;
    if (is_clear_accept)
    {
        struct ksock_accept_node *p = NULL;
        while(__k_accept_pop(hd, p) == KSOCK_SUC)
        {
            //TODO 断开连接 p->fd
            close(p->fd);
            p = NULL;
        }
    }
    return KSOCK_SUC;
}

int k_get_accept_node(const int hd, struct ksock_accept_node *node)
{
    if (KSOCK_ERR == __check_hd(hd))
    {
        return KSOCK_ERR;
    }

    if (__k_accept_pop(hd, node) == KSOCK_SUC)
    {
        return KSOCK_SUC;
    }
    else
    {
        return KSOCK_ERR;
    }
}

int k_get_connect_node(const int hd, struct kscok_connect_node *node)
{
    if (KSOCK_ERR == __check_hd(hd))
    {
        return KSOCK_ERR;
    }

    if (NULL == _hd_array[hd]->connect_node)
    {
        _error_msg = "connect node not exist!";
        return KSOCK_ERR;
    }
    node->fd = _hd_array[hd]->connect_node->fd;
    node->hd = _hd_array[hd]->connect_node->hd;
    node->addr_in = _hd_array[hd]->connect_node->addr_in;
    return KSOCK_SUC;
}

int k_connect(const int hd, const char *address, const uint16_t port, const short family)
{
    if (KSOCK_ERR == __check_hd(hd))
    {
        return KSOCK_ERR;
    }
    
    struct kscok_connect_node *p = (struct kscok_connect_node*)malloc(sizeof(struct ksock_accept_node));
    p->fd = _hd_array[hd]->fd;
    p->hd = hd;

    struct sockaddr_in addr_in = {0};
    addr_in.sin_family = family;
    addr_in.sin_port = htons(port);
    addr_in.sin_addr.s_addr = inet_addr(address);

    p->addr_in.sin_family = addr_in.sin_family;
    p->addr_in.sin_port = addr_in.sin_port;
    p->addr_in.sin_addr.s_addr = addr_in.sin_addr.s_addr;
    int ret = connect(p->fd, (const struct sockaddr *)&addr_in, sizeof(addr_in));
    if (ret < 0)
    {
        perror("k_connect");
        _error_msg = "connect fail, error message had print!";
        return KSOCK_ERR;
    }
    _hd_array[hd]->connect_node = p;
    _hd_array[hd]->state = KSOCK_STATE_ACTIVE;
    return KSOCK_SUC;
}

int k_send(const int fd, void *buf, size_t len, int flag)
{
    return send(fd, buf, len, flag); 
}

int k_recv(const int fd, void *buf, size_t len, int flag)
{
    return recv(fd, buf, len, flag); 
}


int main(int argc, char const *argv[])
{
    /* code */
    return 0;
}