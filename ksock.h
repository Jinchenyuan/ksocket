
#ifndef __KSOCK_H_
#define __KSOCK_H_

#include <sys/socket.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

enum ksock_mode
{
    KSOCK_UNKNOW = -1,
    KSOCK_SERVER = 0,
    KSOCK_CLIENT = 1,
};

enum ksock_err
{
    KSOCK_SUC = 0,
    KSOCK_ERR = -1,
};

enum ksock_af
{
    KSOCK_INET = AF_INET,
    KSOCK_INET6 = AF_INET6,
};

enum ksock_proto
{
    KSOCK_TCP = SOCK_STREAM,
    KSOCK_UDP = SOCK_DGRAM,
};

/**
 * 一旦socket fd开启accept，就意味着状态为KSOCK_STATE_ACTIVE
 * */
enum ksock_state
{
    KSOCK_STATE_ERR = 0,
    KSOCK_STATE_SLEEP,
    KSOCK_STATE_LISTEN,
    KSOCK_STATE_ACTIVE,
};

struct ksock_init{
    short af;
    short proto;
};

struct ksock_accept_node{
    int fd;
    uint16_t port;
};

struct ksock_node{
    int fd;
    short state;
    short mode;
    uint16_t port;
    struct ksock_accept_node *accpet_head;
    struct ksock_accept_node *accpet_tail;
    pthread_t accept_thread;
    struct ksock_init init;
};

#ifdef __cplusplus
}
#endif

#endif /* __KSOCK_H_ */