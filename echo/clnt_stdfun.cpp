#include <arpa/inet.h>
#include <bits/types/FILE.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void error_handling(const char *);
constexpr const char *address = "127.0.0.1";
constexpr ushort port = 9091;
constexpr int buff_size = 1024;

int main(int argc, char *argv[]) {
    sockaddr_in srv_addr;
    int clnt_sock;

    clnt_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
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

    FILE *readfp = fdopen(clnt_sock, "r");
    FILE *writefp = fdopen(clnt_sock, "w");

    char message[buff_size];
    while (true) {
        // client need to read data from user input
        fputs("Input message(Q to quit): ", stdout);
        fgets(message, buff_size, stdin);
        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n")) {
            break;
        }
        fputs(message, writefp);
        fflush(writefp);
        fgets(message, buff_size, readfp);
        printf("Message from server: %s", message);
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