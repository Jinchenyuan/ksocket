
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
    KSOCK_ACCEPT_OVERFLOW,
};

struct ksock_init{
    short af;
    short proto;
};

struct ksock_accept_node{
    int fd;
    int hd;
    struct sockaddr_in addr_in;
    struct ksock_accept_node *next;
};

struct ksock_node{
    int fd;
    short state;
    short mode;
    uint16_t port;
    struct ksock_accept_node *accept_head;
    struct ksock_accept_node *accept_tail;
    int accept_count;
    pthread_t accept_thread;
    struct ksock_init init;
};

static char *_error_msg;
/**
 * 当错误信息被设置，错误信息可以被打印
 * @param msg 错误信息标志
*/
static void ksock_perror(const char *msg);

/**
 * 创建socket fd
 * @param i 用于初始化socket
 * @return 当成功是返回hd，该hd用于操作socket的唯一标志符，错误时，返回KSOCK_ERR，错误信息将被设置
*/
static int k_socket(const struct ksock_init i);

/**
 * 启用listen
 * @param hd socket 唯一标志
 * @param address   绑定的ip地址
 * @param port      绑定的端口号
 * @param family    使用的ip族
 * @return          成功时返回KSOCK_SUC，该socket的状态作相应的改改；错误时则返回KSOCK_ERR，错误信息将被设置
*/
static int k_listen(const int hd, const char *address, const uint16_t port, const short family);

/**
 * 启用accept，注意：调用该函数不阻塞
 * @param hd    socket标志
 * @return      成功时返回KSOCK_SUC；错误时则返回KSOCK_ERR，错误信息将被设置
*/
static int k_accept(const int hd);

/**
 * 取消accept
 * @param hd                socket标志
 * @param is_clear_accept   是否清除已经接收但没有处理的socket连接 1清除，非1不清除
 * @return                  成功时返回KSOCK_SUC；错误时则返回KSOCK_ERR，错误信息将被设置
*/
static int k_accept_cancel(const int hd, int is_clear_accept);

#ifdef __cplusplus
}
#endif

#endif /* __KSOCK_H_ */