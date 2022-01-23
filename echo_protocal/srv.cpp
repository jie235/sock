#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

void error_handling(const char *);
int calculate(char[], char);

constexpr ushort port = 9090;
constexpr int queue_size = 5;
constexpr int buff_size = 1024;
constexpr int OPSZ = 4;

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

    socklen_t srv_addr_len = sizeof(srv_addr);
    if (bind(srv_sock, reinterpret_cast<struct sockaddr *>(&srv_addr),
             srv_addr_len) == -1) {
        error_handling("bind() error");
    }

    if (listen(srv_sock, queue_size) == -1) {
        error_handling("listen() error");
    }

    socklen_t clnt_addr_len = sizeof(clnt_addr_len);
    clnt_sock =
        accept(srv_sock, reinterpret_cast<struct sockaddr *>(&clnt_addr),
               &clnt_addr_len);
    if (clnt_sock == -1) {
        error_handling("accept() error");
    }

    char message[buff_size];
    read(clnt_sock, &message[0], 1);
    printf("loop number: %d\n", message[0] - '0');
    int i;
    for (i = 0; i < message[0] - '0'; ++i) {
        read(clnt_sock, &message[OPSZ * i + 1], OPSZ);
    }
    read(clnt_sock, &message[OPSZ * i + 1], 1);

    int res = calculate(message, message[OPSZ * i + 1]);
    //最后一个参数字节长度应该用sizeof运算符计算，同时注意第二个参数是char指针，不是int指针
    write(clnt_sock, (char *)&res, sizeof(res));

    close(clnt_sock);
    close(srv_sock);
    return 0;
}

int calculate(char message[], char op) {
    int res = message[1];
    printf("initial res number: %d\n", res);
    printf("operator: %c\n", op);
    for (int i = 1; i < message[0] - '0'; i++) {
        printf("current loop value number: %d\n", message[OPSZ * i + 1]);
        switch (op) {
        case '+':
            res += message[OPSZ * i + 1];
            break;
        case '-':
            res -= message[OPSZ * i + 1];
            break;
        case '*':
            res *= message[OPSZ * i + 1];
            break;
        }
    }
    printf("pre return res number: %d\n", res);
    return res;
}

void error_handling(const char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}