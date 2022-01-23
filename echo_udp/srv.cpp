#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

void error_handling(const char *);

constexpr ushort port = 9091;
constexpr int buff_size = 1024;

int main(int argc, char *argv[]) {
    sockaddr_in srv_addr, clnt_addr;
    int srv_sock;

    srv_sock = socket(PF_INET, SOCK_DGRAM, 0);
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

    char message[buff_size];
    int str_len;
    socklen_t clnt_addr_len;
    while (true) {
        clnt_addr_len = sizeof(clnt_addr);
        str_len =
            recvfrom(srv_sock, message, buff_size, 0,
                     reinterpret_cast<sockaddr *>(&clnt_addr), &clnt_addr_len);
        //与redvfrom配对的是sendto，不要用write!!
        sendto(srv_sock, message, str_len, 0,
               reinterpret_cast<sockaddr *>(&clnt_addr), sizeof(clnt_addr));
    }

    close(srv_sock);
    return 0;
}

void error_handling(const char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}