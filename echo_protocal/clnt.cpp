#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void error_handling(const char *);

constexpr const char *address = "127.0.0.1";
constexpr ushort port = 9090;
constexpr int buff_size = 1024;
constexpr int OPSZ = 4;

int main(int argc, char *argv[]) {
    sockaddr_in srv_addr;
    int clnt_sock;

    clnt_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clnt_sock == -1) {
        error_handling("socket() error");
    }

    memset(&srv_addr, 0, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_addr.s_addr = inet_addr(address);
    srv_addr.sin_port = htons(port);

    socklen_t clnt_addr_len = sizeof(srv_addr);
    if (connect(clnt_sock, reinterpret_cast<struct sockaddr *>(&srv_addr),
                clnt_addr_len) == -1) {
        error_handling("connect() error");
    }

    printf("starting sending data...\n");
    printf("Input loop number: ");

    // fgetc()得到一个字符的acsii码，相对来说安全一些，但是当前可以简单化这种情况
    //  char c = static_cast<char>(fgetc(stdin));

    //第一个操作数说明可以放到整个字符串中一起传输，不必单独传递
    // char c = std::cin.get();
    // write(clnt_sock, &c, 1);

    char message[buff_size];
    std::cin.get(message[0]); // read a char

    int n, i;
    for (i = 0; i < message[0] - '0'; i++) {
        printf("Operand %d: ", i + 1);
        scanf("%d", (int *)&message[i * OPSZ + 1]);
    }
    printf("Input operator: ");
    //因为上一次读整数之后，把i的值又增加了1，所以i*OPSZ+1的位置是空的，不应该存到i*OPSZ+2的位置
    //先清除缓冲区中的换行符
    // std::cin.get();
    scanf("\n%c", &message[i * OPSZ + 1]);
    // printf("last character compared result with '\n': %d",
    // message[i*OPSZ+1]=='\n');
    //输出 sizeof (array) 输出的是容量
    // write(clnt_sock, message, sizeof(message));
    write(clnt_sock, message, i * OPSZ + 2);

    read(clnt_sock, &n, sizeof(int));
    printf("calculate result: %d\n", n);
    return 0;
}

void error_handling(const char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}