#include <arpa/inet.h>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>

void error_handling(const char *);

int main(int argc, char *argv[]) {
    int tcp_sock, udp_sock;
    tcp_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (tcp_sock == -1) {
        error_handling("socket() error");
    }
    udp_sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udp_sock == -1) {
        error_handling("socket() error");
    }

    int sock_type;
    socklen_t optlen;
    getsockopt(tcp_sock, SOL_SOCKET, SO_TYPE, &sock_type, &optlen);

    printf("tcp sock type: %d\n", sock_type);
    getsockopt(udp_sock, SOL_SOCKET, SO_TYPE, &sock_type, &optlen);
    printf("udp sock type: %d\n", sock_type);

    return 0;
}

void error_handling(const char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}