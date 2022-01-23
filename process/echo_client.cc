#include <arpa/inet.h>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>

void error_handling(const char *);

constexpr const char *address = "127.0.0.1";
constexpr ushort port = 9091;
constexpr int buff_size = 0x01 << 10;

int main(int argc, char *argv[]) {
    sockaddr_in srv_addr;
    int clnt_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clnt_sock == -1) {
        error_handling("socket() error");
    }

    memset(&srv_addr, 0, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_addr.s_addr = inet_addr(address);
    srv_addr.sin_port = htons(port);

    socklen_t srv_addr_len = sizeof(srv_addr);
    if (connect(clnt_sock, reinterpret_cast<sockaddr *>(&srv_addr),
                srv_addr_len) == -1) {
        error_handling("connect() error");
    }

    char message[buff_size];
    while (true) {
        fputs("Input message(Q to quit): ", stdout);
        fgets(message, buff_size, stdin);
        if (!strcmp(message, "Q\n") || !strcmp(message, "q\n")) {
            break;
        }
        size_t str_len = strlen(message);
        write(clnt_sock, message, str_len);
        int recv_len = 0, recv_cnt;
        while (recv_len < str_len) {
            recv_cnt = read(clnt_sock, message, str_len);
            if (recv_cnt == -1) {
                error_handling("read() error");
            }
            recv_len += recv_cnt;
        }
        message[str_len] = 0;
        printf("Message from server: %s", message);
    }
    close(clnt_sock);
    return 0;
}

void error_handling(const char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}