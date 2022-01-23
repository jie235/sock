#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

constexpr const char *address = "127.0.0.1";
constexpr ushort port = 9091;
constexpr int buff_size = 0x01 << 8;

char msg[buff_size];
char name[] = "[DEFAULT]";

void error_handling(const char *);
void *send_msg(void *);
void *recv_msg(void *);

// FIXME it seems that this multi-thread client has some logical or lingustic
// problem: it will always not receiving message from any other client or
// server(although you may think it is not sure for now client or server which
// will be responsible for the problem, keep looking), but return's the expected
// echo message when using single process echo client as the "chat client".
int main(int argc, char *argv[]) {
    int clnt_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clnt_sock == -1) {
        error_handling("socket() error");
    }
    sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(address);
    serv_addr.sin_port = htons(port);

    if (connect(clnt_sock, reinterpret_cast<sockaddr *>(&serv_addr),
                sizeof(serv_addr)) == -1) {
        error_handling("connect() error");
    }

    pthread_t send_thread, recv_thread;
    void *thread_rtn;
    pthread_create(&send_thread, nullptr, send_msg,
                   static_cast<void *>(&clnt_sock));
    pthread_create(&recv_thread, nullptr, recv_msg,
                   static_cast<void *>(&clnt_sock));
    printf("preparing start sending thread\n");
    pthread_join(send_thread, &thread_rtn);
    printf("preparing start recving thread\n");
    pthread_join(recv_thread, &thread_rtn);
    printf("preparing free resource\n");
    close(clnt_sock);
    return 0;
}

void *send_msg(void *arg) {
    int clnt_sock = *(reinterpret_cast<int *>(&arg));
    char name_msg[sizeof(name) + buff_size + 1];
    while (true) {
        fgets(msg, buff_size, stdin);
        if (!strcmp(msg, "q\n") || !strcmp(msg, "Q\n")) {
            break;
        }
        sprintf(name_msg, "%s %s", name, msg);
        write(clnt_sock, name_msg, strlen(name_msg));
    }
    // if exit the child thread in loop with exit(int) function, add a
    // close(fd), else only need a call of shutdown() (I guess)
    // FIXME why should I free the resource in this thread-runing function?
    // Wouldn't it close in the main function?
    fputs("shuting down output stream", stdout);
    fputc('\n', stdout);
    shutdown(clnt_sock, SHUT_WR);
    return nullptr;
}

void *recv_msg(void *arg) {
    int clnt_sock = *(reinterpret_cast<int *>(&arg));
    char name_msg[sizeof(name) + buff_size];
    size_t str_len;
    while (true) {
        str_len = read(clnt_sock, name_msg, sizeof(name) + buff_size);
        if (str_len == -1) {
            // this is a rvalue's cast, code goes like this cause I don't have a
            // proper way to return this value
            return (void *)-1;
        }
        name_msg[str_len] = 0;
        fputs(name_msg, stdout);
    }
    printf("preparing end recving process\n");
    return nullptr;
}

void error_handling(const char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}