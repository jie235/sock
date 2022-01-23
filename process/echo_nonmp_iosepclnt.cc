#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
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

    if (connect(clnt_sock, reinterpret_cast<sockaddr *>(&srv_addr),
                sizeof(srv_addr)) == -1) {
        error_handling("connect() error");
    }

    FILE *readfp = fdopen(clnt_sock, "r");
    FILE *writefp = fdopen(dup(clnt_sock), "w");

    char message[buff_size];
    // printf("praparing into loop...\n");
    while (true) {
        fputs("Input message(Q to quit): ", stdout);
        // fflush(stdout); // use to debug
        fgets(message, buff_size, stdin);
        fputs(message, writefp);
        fflush(writefp);
        if (fgets(message, buff_size, readfp) == nullptr) {
            break;
        }
        printf("Message from server: %s", message);
    }
    strcpy(message, "Thank you!");
    fputs(message, writefp);

    fclose(readfp);
    fclose(writefp);
    return 0;
}

void error_handling(const char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}