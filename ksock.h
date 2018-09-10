
#ifndef __KSOCK_H_
#define __KSOCK_H_

#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>

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
    KSOCK_RECV_OVERFLOW,
};

struct ksock_msg{
    void *buf;
    size_t len;
    struct ksock_msg *next;
};

struct ksock_init{
    short af;
    short proto;
};

struct ksock_connect_node{
    int fd;
    int hd;
    short state;
    struct sockaddr_in addr_in;
    struct ksock_connect_node *next;
    struct ksock_connect_node *last;

    struct ksock_msg *msg_head;
    struct ksock_msg *msg_tail;
    int recv_count;
    pthread_t recv_thread;
    long nd;
};

struct ksock_node
{
    int fd;
    short state;
    short mode;
    uint16_t port;
    struct ksock_connect_node *accept_head;
    struct ksock_connect_node *accept_tail;
    int accept_count;
    int accept_cnt;
    struct ksock_init init;

    struct ksock_connect_node *connect_node;
};

char *_error_msg;
/**
 * 当错误信息被设置，错误信息可以被打印
 * @param msg 错误信息标志
*/
void k_perror(const char *msg);

// inline int __check_hd(const int hd);

/**
 * 创建socket fd
 * @param i 用于初始化socket
 * @return 当成功是返回hd，该hd用于操作socket的唯一标志符，错误时，返回KSOCK_ERR，错误信息将被设置
*/
int k_socket(const struct ksock_init i);

/**
 * 启用listen
 * @param hd socket 唯一标志
 * @param address   绑定的ip地址
 * @param port      绑定的端口号
 * @param family    使用的ip族
 * @return          成功时返回KSOCK_SUC，该socket的状态作相应的更改；错误时则返回KSOCK_ERR，错误信息将被设置
*/
int k_listen(const int hd, const char *address, const uint16_t port, const short family);

/**
 * socket connect
 * @param hd            socket 唯一标志
 * @param address       连接的ip地址
 * @param port          连接的端口号
 * @param family        使用的ip族
 * @return              成功时返回KSOCK_SUC，该socket的状态作相应的更改；错误时则返回KSOCK_ERR，错误信息将被设置
*/
int k_connect(const int hd, const char *address, const uint16_t port, const short family);

/**
 * 启用accept，注意：调用该函数不阻塞
 * @param hd    socket标志
 * @return      成功时返回KSOCK_SUC；错误时则返回KSOCK_ERR，错误信息将被设置
*/
int k_accept(const int hd);

/**
 * 取消accept
 * @param hd                socket标志
 * @param is_clear_accept   是否清除已经接收但没有处理的socket连接 1清除，非1不清除
 * @return                  成功时返回KSOCK_SUC，状态回到listen；错误时则返回KSOCK_ERR，错误信息将被设置
*/
int k_accept_cancel(const int hd, int is_clear_accept);

/**
 * 获取accept node   注意：一旦accept node 被取走，拿到的一方必须为后续对该node做的做的操作负责
 * @param hd        socket标志
 * @param nd        用于接收结果的参数，此参数是一个句柄，是之后操作连接的唯一标志
 * @return          成功时返回KSOCK_SUC；错误时则返回KSOCK_ERR，错误信息将被设置     
*/
int k_get_accept_node(const int hd, long *nd);

/**
 * 移除accept node
 * @param nd        accept node句柄
 * @return          成功时返回KSOCK_SUC；错误时则返回KSOCK_ERR，错误信息将被设置          
*/
int k_remove_accept_node(const long nd);

/**
 * 获取connect node
 * @param hd        socket标志
 * @param nd        用于接收结果的参数，此参数是一个句柄，是之后操作连接的唯一标志
 * @return          成功时返回KSOCK_SUC；错误时则返回KSOCK_ERR，错误信息将被设置          
*/
int k_get_connect_node(const int hd, long *nd);

/**
 * send 与socket send一致，可参阅
 * @param nd        已经连接的nd
 * @param buf       发送的缓存
 * @param len       发送的长度
 * @param flag      socket send flag
 * @return          实际发送的长度
*/
int k_send(const long nd, void *buf, size_t len, int flag);

/**
 * recv 与socket recv一致，可参阅 ？要不要保留recv的阻塞性 ？目前倾向于保留，把处理消息的主动性留给调用者
 * @param nd        已连接的nd
 * @param buf       recv的缓存
 * @param len       recv的缓存区最大值
 * @param flag      socket recv flag
 * @return          实际接收的长度
*/
int k_recv(const long nd, size_t len, int flag);

/**
 * 获取消息
 * @param nd                已连接的nd
 * @param msg               接收消息
 * @return                  成功时返回KSOCK_SUC；错误时则返回KSOCK_ERR，错误信息将被设置 
*/
int k_get_recv_msg(const long nd, struct ksock_msg *msg);

/**
 * 取消recv
 * @param nd                已连接的nd
 * @param is_clear_recv     是否丢弃已经接收到队里的消息
 * @return                  成功时返回KSOCK_SUC；错误时则返回KSOCK_ERR，错误信息将被设置 
*/
int k_recv_cancel(const long nd, int is_clear_recv);

/**
 * socket close    注意，一旦关闭，该hd将废弃。
 * @param hd       socket标志
 * @return          成功时返回KSOCK_SUC；错误时则返回KSOCK_ERR，错误信息将被设置 
*/
int k_close(const int hd);

#ifdef __cplusplus
}
#endif

#endif /* __KSOCK_H_ */