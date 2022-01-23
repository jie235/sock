#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

void error_handling(const char *);

constexpr ushort port = 9091;
constexpr int queue_size = 5;
constexpr int buff_size = 0x01 << 5;

int main(int argc, char *argv[]) {
    sockaddr_in send_addr, recv_addr;
    int send_sock, recv_sock;

    recv_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (recv_sock == -1) {
        error_handling("socket() error");
    }
    memset(&recv_addr, 0, sizeof(recv_addr));
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    recv_addr.sin_port = htons(port);

    if (bind(recv_sock, reinterpret_cast<sockaddr *>(&recv_addr),
             sizeof(recv_addr)) == -1) {
        error_handling("bind() error");
    }
    if (listen(recv_sock, queue_size) == -1) {
        error_handling("listen() error");
    }

    socklen_t send_addr_sz = sizeof(send_addr);
    send_sock = accept(recv_sock, reinterpret_cast<sockaddr *>(&send_addr),
                       &send_addr_sz);
    if (send_sock == -1) {
        error_handling("accept() error");
    }

    int str_len;
    char message[buff_size];
    while (true) {
        // read action will not clear cache(already read data)
        str_len =
            recv(send_sock, message, buff_size - 1, MSG_PEEK | MSG_DONTWAIT);
        if (str_len > 0) {
            break;
        }
    }
    message[str_len] = 0;
    printf("Buffering %d bytes: %s \n", str_len, message);
    str_len = recv(recv_sock, message, buff_size - 1, 0);
    message[str_len] = 0;
    printf("Read again: %s\n", message);
    close(send_sock);
    close(recv_sock);
    return 0;
}

void error_handling(const char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}