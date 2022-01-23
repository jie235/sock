#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>

void error_handling(const char *);

int main(int argc, char *argv[]) {
    int srv_sock, clnt_sock;

    // buff designed for tcp
    srv_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (srv_sock == -1) {
        error_handling("socket() error");
    }

    int optvalue;
    socklen_t optlen;
    getsockopt(srv_sock, SOL_SOCKET, SO_SNDBUF, &optvalue, &optlen);
    printf("default send buff size: %d\n", optvalue);

    getsockopt(srv_sock, SOL_SOCKET, SO_RCVBUF, &optvalue, &optlen);
    printf("default receive buff size: %d\n", optvalue);

    // if udp socket has a buff ? yes
    clnt_sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (clnt_sock == -1) {
        error_handling("socket() error at udp");
    }

    getsockopt(clnt_sock, SOL_SOCKET, SO_SNDBUF, &optvalue, &optlen);
    printf("default send buff for udp: %d\n", optvalue);

    getsockopt(clnt_sock, SOL_SOCKET, SO_RCVBUF, &optvalue, &optlen);
    printf("default receive buff for udp: %d\n", optvalue);

    // custom socket buff for tcp
    optvalue = (0x01 << 10) * 3;
    optlen = sizeof(optvalue);
    setsockopt(srv_sock, SOL_SOCKET, SO_SNDBUF, &optvalue, optlen);
    setsockopt(srv_sock, SOL_SOCKET, SO_RCVBUF, &optvalue, optlen);

    // then read the new buff size from socket
    getsockopt(srv_sock, SOL_SOCKET, SO_SNDBUF, &optvalue, &optlen);
    printf("new set send buff size: %d\n", optvalue);

    getsockopt(srv_sock, SOL_SOCKET, SO_RCVBUF, &optvalue, &optlen);
    printf("new set recv buff size: %d\n", optvalue);

    // does udp socket support buff setting? yes
    setsockopt(clnt_sock, SOL_SOCKET, SO_SNDBUF, &optvalue, optlen);
    setsockopt(clnt_sock, SOL_SOCKET, SO_RCVBUF, &optvalue, optlen);
    getsockopt(clnt_sock, SOL_SOCKET, SO_SNDBUF, &optvalue, &optlen);
    printf("new set send buff size for udp: %d\n", optvalue);

    getsockopt(clnt_sock, SOL_SOCKET, SO_RCVBUF, &optvalue, &optlen);
    printf("new set recv buff size for udp: %d\n", optvalue);

    close(srv_sock);
    close(clnt_sock);

    return 0;
}

void error_handling(const char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}