#include <arpa/inet.h>
#include <bits/types/FILE.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

void error_handling(const char *);

constexpr const char *address = "255.255.255.255";
constexpr ushort port = 9091;
constexpr int buff_size = 0x01 << 8;

int main(int argc, char *argv[]) {
    sockaddr_in recv_addr;
    int send_sock;

    memset(&recv_addr, 0, sizeof(recv_addr));
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_addr.s_addr = inet_addr(address);
    recv_addr.sin_port = htons(port);

    send_sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (send_sock == -1) {
        error_handling("socket() error");
    }

    // udp connection disabled broadcast defaultly
    int optval = 1;
    setsockopt(send_sock, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval));
    char message[buff_size];

    FILE *fp = fopen("news.txt", "rt");
    if (fp == nullptr) {
        error_handling("fopen() error");
    } else {
        printf("open file successful\n");
    }
    while (true) {
        if (feof(fp)) {
            break;
        }
        fgets(message, buff_size - 1, fp);
        sendto(send_sock, message, strlen(message), 0,
               reinterpret_cast<sockaddr *>(&recv_addr), sizeof(recv_addr));
    }
    fclose(fp);
    close(send_sock);
    return 0;
}

void error_handling(const char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}
