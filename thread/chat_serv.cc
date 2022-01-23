#include <arpa/inet.h>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void error_handling(const char *);

constexpr int queue_size = 5;
constexpr ushort port = 9091;
constexpr int max_client = 128;
constexpr int buff_size = 0x01 << 8;
int clnt_socks[max_client];
int cnlt_cnt = 0;

pthread_mutex_t mutex;
void *handle_clnt(void *);
void send_msg(char *, int);

int main(int argc, char *argv[]) {
    int serv_sock, clnt_sock;
    serv_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serv_sock == -1) {
        error_handling("socket() error");
    }

    sockaddr_in serv_addr, clnt_addr;
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

    pthread_mutex_init(&mutex, nullptr);
    pthread_t t_pid;
    socklen_t clnt_addr_size;

    // code works in this way: each time when connectted a socket connection,
    // create a thread to handle the request. when the connection does not
    // receive any message, close the connection
    while (true) {
        clnt_addr_size = sizeof(clnt_addr);
        clnt_sock = accept(serv_sock, reinterpret_cast<sockaddr *>(&clnt_addr),
                           &clnt_addr_size);
        pthread_mutex_lock(&mutex);
        clnt_socks[cnlt_cnt++] = clnt_sock;
        pthread_mutex_unlock(&mutex);
        pthread_create(&t_pid, nullptr, handle_clnt,
                       static_cast<void *>(&clnt_sock));
        pthread_detach(t_pid);
        printf("Connected %d clients.\n", cnlt_cnt);
    }
    close(serv_sock);
    return 0;
}

void *handle_clnt(void *param) {
    int clnt_sock = *(reinterpret_cast<int *>(param));
    size_t str_len = 0;
    char msg[buff_size];
    printf("preparing writing throuth %d.\n", clnt_sock);
    while ((str_len = read(clnt_sock, msg, buff_size)) != 0) {
        printf("data length: %zd", str_len);
        send_msg(msg, str_len);
    }
    // moving forward sockets requires atomatic
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < cnlt_cnt; ++i) {
        if (clnt_sock == clnt_socks[i]) {
            while (i < cnlt_cnt - 1) {
                clnt_socks[i] = clnt_socks[i + 1];
                ++i;
            }
            break;
        }
    }
    --cnlt_cnt;
    pthread_mutex_unlock(&mutex);
    close(clnt_sock);
    return nullptr;
}

void send_msg(char *msg, int len) {
    // cause the clnt_socks and cnlt_cnt consists of critical region, use a
    // lock here
    printf("locking\n");
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < cnlt_cnt; ++i) {
        write(clnt_socks[i], msg, len);
    }
    pthread_mutex_unlock(&mutex);
}

void error_handling(const char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}