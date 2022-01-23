#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void error_handling(const char *);
void read_data(int, char[]);
void write_data(int, char[]);

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

    char message[buff_size];
    pid_t pid = fork();
    // cause data read and write are separated in different process, input
    // message and output message will display on the same line
    if (pid == 0) {
        write_data(clnt_sock, message);
    } else {
        read_data(clnt_sock, message);
    }

    return 0;
}

void read_data(int sock, char message[]) {
    // set the tail of message to 0 in order to ensure writing process writes
    // exactly what reading process has read.
    while (true) {
        int str_len = read(sock, message, buff_size);
        if (str_len == 0) {
            return;
        }
        message[str_len] = 0;
        printf("Message from server: %s", message);
    }
}

void write_data(int sock, char message[]) {
    while (true) {
        fputs("Input message(Q to quit): ", stdout);
        fgets(message, buff_size, stdin);
        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n")) {
            // not close fd, just close output stream
            shutdown(sock, SHUT_WR);
            return;
        }
        write(sock, message, strlen(message));
    }
}

void error_handling(const char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}