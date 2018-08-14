#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define IP_ADDR "192.168.1.102"

int main(int argc, char const *argv[])
{
    /*
    in_addr_t addr = 0;
    struct in_addr i_addr = {0};
    // addr = inet_addr(IP_ADDR);
    inet_pton(AF_INET, IP_ADDR, &i_addr);
    // printf("addr = 0x%x\n", addr);
    printf("addr = 0x%x\n", i_addr.s_addr);
    */
    char addr_str[50] = {0};
    struct in_addr i_addr = {0};
    i_addr.s_addr = 0x6601a8c0;
    inet_ntop(AF_INET, &i_addr, addr_str, sizeof(addr_str));
    printf("addr = %s\n", addr_str);
    return 0;
}
