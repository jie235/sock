#include <arpa/inet.h>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

void error_handling(const char *);
void urg_handler(int sig);

constexpr ushort port = 9091;
constexpr int queue_size = 5;
constexpr int buff_size = 0x01 << 5;

int recv_sock, send_sock;

int main(int argc, char *argv[]) {
    sockaddr_in recv_addr, send_addr;

    struct sigaction act;
    act.sa_handler = urg_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

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

    char message[buff_size];
    socklen_t send_addr_len = sizeof(send_addr);
    send_sock = accept(recv_sock, reinterpret_cast<sockaddr *>(&send_addr),
                       &send_addr_len);
    if (send_sock == -1) {
        error_handling("accept() error");
    }

    // FIXME why does not the setting of emergency message handler work? I
    // placed it ahead of signal register function already
    //  set owner(more accuratly speaking, signal dealer) of send_sock to the
    //  process which use the value of getpid()'s return value as process id
    fcntl(send_sock, F_SETOWN, getpid());

    int state = sigaction(SIGURG, &act, 0);

    int str_len;

    // guess urgent message need to deal with signal
    while ((str_len = recv(send_sock, message, buff_size, 0)) != 0) {
        if (str_len == -1) {
            continue;
        }
        message[str_len] = 0;
        puts(message);
    }
    close(send_sock);
    close(recv_sock);
    return 0;
}

void urg_handler(int sig) {
    int str_len;
    char buff[buff_size];
    str_len = recv(send_sock, buff, buff_size - 1, MSG_OOB);
    buff[str_len] = 0;
    printf("Urgent message: %s\n", buff);
}

void error_handling(const char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}