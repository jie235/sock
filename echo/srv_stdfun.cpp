#include <arpa/inet.h>
#include <bits/types/FILE.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>

void error_handling(const char *);

constexpr ushort port = 9091;
constexpr int queue_size = 5;
constexpr int buff_size = 1024;

int main(int argc, char *argv[]) {
    sockaddr_in srv_addr, clnt_addr;
    int clnt_sock, srv_sock;

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
    if (listen(srv_sock, queue_size) == 5) {
        error_handling("listen() error");
    }

    socklen_t clnt_addr_len = sizeof(clnt_addr);
    clnt_sock = accept(srv_sock, reinterpret_cast<sockaddr *>(&clnt_addr),
                       &clnt_addr_len);
    if (clnt_sock == -1) {
        error_handling("accept() error");
    }

    FILE *readfp = fdopen(clnt_sock, "r");
    FILE *writefp = fdopen(clnt_sock, "w");
    while (!feof(readfp)) { // condition
        char message[buff_size];
        fgets(message, buff_size, readfp);
        fputs(message, writefp);
        fflush(writefp);
    }
    fclose(readfp);
    fclose(writefp);
    return 0;
}

void error_handling(const char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}