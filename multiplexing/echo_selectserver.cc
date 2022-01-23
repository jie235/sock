#include <arpa/inet.h>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

void error_handling(const char *);

constexpr ushort port = 9091;
constexpr int buff_size = 1024;
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

    timeval timeout;
    int fd_max = srv_sock, result;
    fd_set reads, cpy_reads;
    FD_ZERO(&reads);
    FD_SET(srv_sock, &reads);

    char message[buff_size];
    while (true) {
        cpy_reads = reads;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        // nfds need to be set as fs_max+1: regard fd_set as a bitset, the first
        // one is 0 but then has 1 record, ..., the value N then will has a
        // index as N+1
        result = select(fd_max + 1, &cpy_reads, 0, 0, &timeout);
        if (result == -1) {
            break;
        }
        if (result == 0) {
            continue;
        }
        for (int i = 0; i < fd_max + 1; ++i) {
            if (FD_ISSET(i, &cpy_reads)) {
                // connection request
                if (i == srv_sock) {
                    socklen_t clnt_addr_len = sizeof(clnt_addr);
                    clnt_sock = accept(srv_sock,
                                       reinterpret_cast<sockaddr *>(&clnt_addr),
                                       &clnt_addr_len);
                    FD_SET(clnt_sock, &reads);
                    if (fd_max < clnt_sock) {
                        fd_max = clnt_sock;
                    }
                    printf("connected client: %d\n", clnt_sock);
                } else { // read data
                    // it has only one process reading data eath time, so
                    // use a shared buffer is ok here
                    size_t str_len = read(i, message, buff_size);
                    if (str_len == 0) {
                        FD_CLR(i, &reads);
                        close(i);
                        printf("close client: %d\n", i);
                    } else {
                        write(i, message, str_len);
                    }
                }
            }
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
