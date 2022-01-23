#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>

void error_handling(const char *);

constexpr ushort port = 9091;
constexpr int buff_size = 0x01 << 10;
constexpr int queue_size = 5;

int main(int argc, char *argv[]) {
    sockaddr_in clnt_addr, srv_addr;
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
    if (listen(srv_sock, queue_size) == -1) {
        error_handling("listen() error");
    }

    socklen_t clnt_addr_len = sizeof(clnt_addr);
    clnt_sock = accept(srv_sock, reinterpret_cast<sockaddr *>(&clnt_addr),
                       &clnt_addr_len);
    if (clnt_sock == -1) {
        error_handling("accept() error");
    }

    FILE *readfp = fdopen(clnt_sock, "r");
    FILE *writefp = fdopen(dup(clnt_sock), "w");

    char message[buff_size];
    while (true) {
        // if wish client replys a "Thank you" after the server finished
        // transfering, need to shutdown server's output. If the client's output
        // was shutdown, it cannot send a "Thank you" reply; If the client's
        // input was shutdown, it does not make sense
        fgets(message, buff_size, readfp);
        if (!strcmp(message, "Q\n") || !strcmp(message, "q\n")) {
            shutdown(fileno(writefp), SHUT_WR);
            break;
        }
        fputs(message, writefp);
        fflush(writefp);
    }
    fgets(message, buff_size, readfp);
    printf("Last message: %s", message);
    fclose(readfp);
    fclose(writefp);
    return 0;
}

void error_handling(const char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}