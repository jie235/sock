#include <arpa/inet.h>
#include <bits/types/FILE.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

void error_handling(const char *);

constexpr ushort port = 9091;
constexpr int buff_size = 30;
constexpr int queue_size = 5;

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

    printf("preparing sending data: \n");

    FILE *fp = fopen("./srv.cpp", "rb");

    char message[buff_size];
    int read_cnt = 0;
    while (true) {
        read_cnt = fread(message, 1, buff_size, fp);
        // in common case, reads buff_size bytes and writes to clnt_sock. when
        // has no enough data, end the loop
        if (read_cnt < buff_size) {
            write(clnt_sock, message, read_cnt);
            break;
        }
        write(clnt_sock, message, buff_size);
    }

    shutdown(clnt_sock, SHUT_WR);
    //都多少，写多少，不要把buff里面都写出去
    int str_len = read(clnt_sock, message, buff_size);
    message[str_len] = 0;
    printf("Post reply from client: %s", message);

    // don't forget to close file stream!
    fclose(fp);
    close(clnt_sock);
    close(srv_sock);
    return 0;
}

void error_handling(const char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}