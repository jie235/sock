#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>

void error_handling(const char *);

constexpr const char *address = "127.0.0.1";
constexpr ushort port = 9091;

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in recv_addr;

    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    memset(&recv_addr, 0, sizeof(recv_addr));
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_addr.s_addr = inet_addr(address);
    recv_addr.sin_port = htons(port);

    if (connect(sock, reinterpret_cast<sockaddr *>(&recv_addr),
                sizeof(recv_addr)) == -1) {
        error_handling("connect() error.");
    }
    write(sock, "123", strlen("123"));
    send(sock, "4", strlen("4"), MSG_OOB);
    write(sock, "567", strlen("567"));
    send(sock, "890", strlen("890"), MSG_OOB);
    close(sock);
    return 0;
}

void error_handling(const char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}