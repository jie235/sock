#include <arpa/inet.h>
#include <asm-generic/errno-base.h>
#include <cerrno> // no such thing as cerror, only cerrno, errno.h, error.h
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

void error_handling(const char *);

constexpr int queue_size = 5;
constexpr ushort port = 9091;
constexpr int buff_size = 0x01 << 8;
constexpr int EPOLL_SIZE = 50;

int main(int argc, char *argv[]) {
    sockaddr_in clnt_addr, serv_addr;
    int clnt_sock, serv_sock;

    serv_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serv_sock == -1) {
        error_handling("socket() error");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);

    if (bind(serv_sock, reinterpret_cast<sockaddr *>(&serv_addr),
             sizeof(serv_addr)) == -1) {
        error_handling("bind() error");
    }
    if (listen(serv_sock, queue_size) == -1) {
        error_handling("listen() error");
    }

    struct epoll_event *ep_events = reinterpret_cast<struct epoll_event *>(
        malloc(sizeof(struct epoll_event) * EPOLL_SIZE));
    struct epoll_event ep_event_in;
    // server sock don't need a edge trigger
    ep_event_in.events = EPOLLIN;
    ep_event_in.data.fd = serv_sock;

    auto ep_fd = epoll_create(EPOLL_SIZE);
    if (ep_fd == -1) {
        error_handling("epoll_create() error");
    }

    if (epoll_ctl(ep_fd, EPOLL_CTL_ADD, serv_sock, &ep_event_in) == -1) {
        error_handling("epoll_ctl() error");
    }

    char message[buff_size];
    int epoll_cnt;

    while (true) {
        epoll_cnt = epoll_wait(ep_fd, ep_events, EPOLL_SIZE, -1);
        if (epoll_cnt == -1) {
            puts("epoll_wait() error");
            break;
        }
        for (int i = 0; i < epoll_cnt; ++i) {
            auto fd = ep_events[i].data.fd;
            if (fd == serv_sock) {
                socklen_t clnt_addr_len = sizeof(clnt_addr);
                clnt_sock =
                    accept(serv_sock, reinterpret_cast<sockaddr *>(&clnt_addr),
                           &clnt_addr_len);
                if (clnt_sock == -1) {
                    puts("accept() error");
                    continue;
                }

                // set socket to non-blocking
                auto flag = fcntl(clnt_sock, F_GETFL, 0);
                fcntl(clnt_sock, F_SETFL, flag | O_NONBLOCK);
                // set edge trigger
                ep_event_in.events = EPOLLIN | EPOLLET;
                ep_event_in.data.fd = clnt_sock;
                epoll_ctl(ep_fd, EPOLL_CTL_ADD, clnt_sock, &ep_event_in);
                printf("connected client: %d\n", clnt_sock);
            } else {
                while (true) {
                    auto str_len = read(fd, message, buff_size);
                    // printf("read %ld bytes.\n", str_len);
                    if (str_len < 0) {
                        // no data to read, but keep the connection
                        if (errno == EAGAIN) {
                            // all data in input buffer is read
                            break;
                        }
                    }
                    if (str_len == 0) {
                        // close the connection
                        epoll_ctl(ep_fd, EPOLL_CTL_DEL, fd, NULL);
                        close(fd);
                        printf("close client: %d\n", fd);
                        break;
                    }
                    write(fd, message, str_len);
                }
            }
        }
    }
    close(serv_sock);
    close(ep_fd);
    return 0;
}

void error_handling(const char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}