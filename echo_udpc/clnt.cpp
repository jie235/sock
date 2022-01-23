#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <system_error>
#include <unistd.h>

void error_handling(const char *);

constexpr const char *address = "127.0.0.1";
constexpr ushort port = 9091;
constexpr int buff_size = 1024;

int main(int argc, char *argv[]) {
    sockaddr_in srv_addr;
    int clnt_sock;

    clnt_sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (clnt_sock == -1) {
        error_handling("socket() error");
    }

    memset(&srv_addr, 0, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_addr.s_addr = inet_addr(address);
    srv_addr.sin_port = htons(port);

    if (connect(clnt_sock, reinterpret_cast<sockaddr *>(&srv_addr),
                sizeof(srv_addr)) == -1) {
        error_handling("connect() error");
    }

    char message[buff_size];
    socklen_t srv_addr_len = sizeof(srv_addr);
    while (true) {
        fputs("Input message (Q to quit): ", stdout);
        fgets(message, buff_size, stdin);
        if (!strcmp("q\n", message) || !strcmp("Q\n", message)) {
            break;
        }
        size_t str_len =
            sendto(clnt_sock, message, strlen(message), 0,
                   reinterpret_cast<sockaddr *>(&srv_addr), sizeof(srv_addr));
        recvfrom(clnt_sock, message, str_len, 0,
                 reinterpret_cast<sockaddr *>(&srv_addr), &srv_addr_len);
        message[str_len] = 0;
        printf("Message fromm server: %s", message);
    }
    close(clnt_sock);
    return 0;
}

void error_handling(const char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}