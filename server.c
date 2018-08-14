#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>

#define SERVER_PORT     6004
#define SERVER_ADDR     "192.168.44.132"
#define LISTEN_NUM      100

char recv_buf[100];

int main(int argc, char const *argv[])
{
    //第一步：先socket打开文件描述符
    int sockfd = -1;
    int accept_fd = -1;
    struct sockaddr_in server_addr = {0};
    struct sockaddr_in client_addr = {0};
    socklen_t len = 0;
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
    while(1)
    {
        //第四步：
        accept_fd = accept(sockfd, (struct sockaddr*)&client_addr, &len);
        printf("accept fd = %d\n", accept_fd);
        
        ret = recv(accept_fd, recv_buf, sizeof(recv_buf), 0);
        printf("recv %d char.\n", ret);
        printf("recv_buf [%s]\n", recv_buf);
    }
    
    return 0;
}
