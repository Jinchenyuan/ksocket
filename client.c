#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <string.h>

#define SERVER_ADDR     "192.168.44.132"
#define SERVER_PORT     6004

char send_buf[100];

int main(int argc, char const *argv[])
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

    //第二步：connect连接服务器
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    ret = connect(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret < 0)
    {
        perror("connect");
        return -1;
    }
    printf("connect success %d\n", ret);

    strcpy(send_buf, "hello world.");
    ret = send(sockfd, send_buf, strlen(send_buf), 0);
    if (ret < 0)
    {
        perror("send");
        return -1;
    }
    printf("send %d char\n", ret);
    return 0;
}
