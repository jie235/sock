#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

constexpr const char *address = "127.0.0.1";
constexpr ushort port = 9091;

void error_handling(const char *);

int main(int argc, char *argv[]) {
    sockaddr_in recv_addr;
    int send_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (send_sock == -1) {
        error_handling("socket() error");
    }
    memset(&recv_addr, 0, sizeof(recv_addr));
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_addr.s_addr = inet_addr(address);
    recv_addr.sin_port = htons(port);
    if (connect(send_sock, reinterpret_cast<sockaddr *>(&recv_addr),
                sizeof(recv_addr)) == -1) {
        error_handling("connect() error");
    }
    write(send_sock, "123", strlen("123"));
    close(send_sock);
    return 0;
}

void error_handling(const char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}