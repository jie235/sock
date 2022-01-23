#include <arpa/inet.h>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

void error_handling(const char *);
void read_childproc(int);

constexpr ushort port = 9091;
constexpr int buff_size = 0x01 << 10;
constexpr int queue_size = 5;

int main(int argc, char *argv[]) {
    sockaddr_in srv_addr, clnt_addr;
    int srv_sock, clnt_sock;
    socklen_t clnt_addr_len;

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

    struct sigaction act;
    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGCHLD, &act, 0);

    pid_t pid;
    char message[buff_size];
    while (true) { // hindsight: server self does not break out the loop. In
                   // this condition, server process will never exit.
        clnt_addr_len = sizeof(clnt_addr);
        clnt_sock = accept(srv_sock, reinterpret_cast<sockaddr *>(&clnt_addr),
                           &clnt_addr_len);
        if (clnt_sock == -1) {
            // don't exit here
            continue;
        } else {
            puts("new client connected.");
        }
        pid = fork();
        if (pid == 0) {
            // close server socket
            close(srv_sock);
            // handle data exchange
            size_t str_len;
            while ((str_len = read(clnt_sock, message, buff_size)) != 0) {
                write(clnt_sock, message, str_len);
            }
            close(clnt_sock);
            return 0;
        } else {
            // close client socket. server does not transfer data
            close(clnt_sock);
        }
    }

    close(srv_sock);
    return 0;
}

void error_handling(const char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}

void read_childproc(int sig) {
    int status;
    pid_t pid = waitpid(-1, &status, WNOHANG);
    if (WIFEXITED(status)) {
        printf("End child process %d\n", pid);
        printf("Child process send: %d\n", WEXITSTATUS(status));
    }
}