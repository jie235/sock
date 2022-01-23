#include <arpa/inet.h>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

void error_handling(const char *);

constexpr const char *address = "127.0.0.1";
constexpr ushort port = 9091;
constexpr int buff_size = 1024;

int main(int argc, char *argv[]) {
    sockaddr_in srv_addr, clnt_addr;
    int clnt_sock;

    clnt_sock = socket(PF_INET, SOCK_DGRAM, 0);
    if (clnt_sock == -1) {
        error_handling("socket() error");
    }

    memset(&srv_addr, 0, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_addr.s_addr = inet_addr(address);
    srv_addr.sin_port = htons(port);

    char message[buff_size], inmsg[buff_size];
    socklen_t clnt_addr_len;
    while (true) {
        printf("Input message (Q to quit): ");
        // fgets() 读取的结果包含最后的换行符，第二个参数指定 max-buff
        fgets(message, buff_size, stdin);
        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n")) {
            break;
        }
        // valid bit nums
        size_t str_len = strlen(message);
        sendto(clnt_sock, message, strlen(message), 0,
               reinterpret_cast<sockaddr *>(&srv_addr), sizeof(srv_addr));
        //发送之后会不会清空发送的数据？
        //不会
        // printf("data in buff after send: %s", message);
        // size_t recv_cnt = 0;
        clnt_addr_len = sizeof(clnt_addr);
        // while (recv_cnt < str_len) {
        //     size_t recv_curr = recvfrom(clnt_sock, message, buff_size, 0,
        //                                 reinterpret_cast<sockaddr
        //                                 *>(&srv_addr), &clnt_addr_len);
        //     if (recv_curr == -1) {
        //         error_handling("recvfrom() error");
        //     }
        //     recv_cnt += recv_curr;
        // }

        // interesting: from address don't need to be specific, it will fill in
        // automatically
        str_len =
            recvfrom(clnt_sock, inmsg, buff_size, 0,
                     reinterpret_cast<sockaddr *>(&clnt_addr), &clnt_addr_len);

        //把有效位的最后一位置空。空字符'\0'的ascii码值为0
        inmsg[str_len] = 0;
        printf("Message from server: %s", inmsg);
    }
    close(clnt_sock);
    return 0;
}

void error_handling(const char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}