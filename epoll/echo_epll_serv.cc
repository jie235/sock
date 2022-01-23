#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

void error_handling(const char *);

constexpr ushort port = 9091;
constexpr int buff_size = 0x01 << 8;
constexpr int queue_size = 5;
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

    int epfd = epoll_create(EPOLL_SIZE);
    if (epfd == -1) {
        error_handling("epoll_create() error");
    }

    // param out
    struct epoll_event *ep_events = reinterpret_cast<struct epoll_event *>(
        malloc(sizeof(struct epoll_event) * EPOLL_SIZE));

    // param in
    struct epoll_event ep_event;
    ep_event.events = EPOLLIN;
    ep_event.data.fd = serv_sock;
    epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &ep_event);

    while (true) {
        char message[buff_size];
        // -1 means until event occurs
        int event_cnt = epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);
        if (event_cnt == -1) {
            error_handling("epoll_wait() error");
            break;
        }
        for (int i = 0; i < event_cnt; ++i) {
            if (ep_events[i].data.fd == serv_sock) {
                socklen_t clnt_addr_len = sizeof(clnt_addr);
                clnt_sock =
                    accept(serv_sock, reinterpret_cast<sockaddr *>(&clnt_addr),
                           &clnt_addr_len);
                if (clnt_sock == -1) {
                    continue;
                }
                ep_event.events = EPOLLIN;
                ep_event.data.fd = clnt_sock;
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &ep_event);
            } else {
                ssize_t str_len =
                    read(ep_events[i].data.fd, message, buff_size);
                if (str_len == 0) {
                    epoll_ctl(epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);
                    close(ep_events[i].data.fd);
                    printf("close client: %d\n", ep_events[i].data.fd);
                } else {
                    write(ep_events[i].data.fd, message, str_len);
                }
            }
        }
    }

    close(serv_sock);
    close(epfd);
    return 0;
}

void error_handling(const char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}