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
constexpr int buff_size = 0x01 << 10;

int main(int argc, char *argv[]) {
    sockaddr_in srv_addr, clnt_addr;
    int srv_sock, clnt_sock;

    srv_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (srv_sock == -1) {
        error_handling("socket() error");
    }

    memset(&srv_addr, 0, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    srv_addr.sin_port = htons(port);

    if (bind(srv_sock, reinterpret_cast<sockaddr *>(&srv_addr),
             sizeof(srv_addr)) == -1) {
        error_handling("bind() error");
    }

    if (listen(srv_sock, queue_size) == -1) {
        error_handling("listen() error");
    }

    socklen_t clnt_addr_len = sizeof(clnt_addr);
    clnt_sock = accept(srv_sock, reinterpret_cast<sockaddr *>(&clnt_addr),
                       &clnt_addr_len);
    if (clnt_sock == -1) {
        error_handling("accept() error");
    }

    // preset optvalue type as bool
    bool optvalue = true;
    int optlen = sizeof(optvalue);
    // setsockopt(srv_sock, SOL_SOCKET, SO_REUSEADDR, &optvalue, optlen);

    char message[buff_size];
    int str_len;
    while ((str_len = read(clnt_sock, message, buff_size)) != 0) {
        write(clnt_sock, message, str_len);
        // also output to stdout
        write(1, message, str_len);
    }

    close(clnt_sock);
    close(srv_sock);
    return 0;
}

void error_handling(const char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}