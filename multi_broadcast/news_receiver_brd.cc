#include <arpa/inet.h>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

void error_handling(const char *);

constexpr ushort port = 9091;
constexpr int buff_size = 0x01 << 8;

int main(int argc, char *argv[]) {
    sockaddr_in recv_addr, send_addr;
    memset(&recv_addr, 0, sizeof(recv_addr));
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    recv_addr.sin_port = htons(port);

    int recv_sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (recv_sock == -1) {
        error_handling("socket() error");
    }

    if (bind(recv_sock, reinterpret_cast<sockaddr *>(&recv_addr),
             sizeof(recv_addr)) == -1) {
        error_handling("bind() error");
    }

    char message[buff_size];
    socklen_t send_addr_len = sizeof(send_addr);
    while (true) {
        ssize_t str_len =
            recvfrom(recv_sock, message, buff_size - 1, 0,
                     reinterpret_cast<sockaddr *>(&send_addr), &send_addr_len);
        if (str_len < 0) {
            break;
        } else {
            printf("Read %zd bytes data\n", str_len);
        }
        message[str_len] = 0;
        fputs(message, stdout);
    }
    close(recv_sock);
    return 0;
}

void error_handling(const char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}