
#ifndef __KSOCK_H_
#define __KSOCK_H_

#include <sys/socket.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint16_t k_port;

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

struct ksock_node{
    int fd;
    short state;
    struct ksock_init init;
};


#ifdef __cplusplus
}
#endif

#endif /* __KSOCK_H_ */