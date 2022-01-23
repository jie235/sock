#include <arpa/inet.h>
#include <bits/types/FILE.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>

void error_handling(const char *);
constexpr const char *address = "239.1.1.2";
constexpr ushort port = 9091;
constexpr int buff_size = 0x01 << 8;

int main(int argc, char *argv[]) {
    sockaddr_in recv_addr;
    int send_sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (send_sock == -1) {
        error_handling("socket() error");
    }
    memset(&recv_addr, 0, sizeof(recv_addr));
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_addr.s_addr = inet_addr(address);
    recv_addr.sin_port = htons(port);

    FILE *fp = fopen("news.txt", "rt");
    if (fp == NULL) {
        error_handling("open source news error");
    } else {
        printf("file is successfully opened.\n");
    }

    // printf("setting ttl value.\n"); // just for debug
    int time_live = 64;
    setsockopt(send_sock, IPPROTO_IP, IP_MULTICAST_TTL, &time_live,
               sizeof(time_live));

    // printf("preparing sending data: \n"); // just for debug
    char message[buff_size];
    while (!feof(fp)) {
        fgets(message, buff_size, fp);
        ssize_t len =
            sendto(send_sock, message, strlen(message), 0,
                   reinterpret_cast<sockaddr *>(&recv_addr), sizeof(recv_addr));
        // write(1, message, strlen(message)); // auch just for debug
        printf("send %zd bytes.\n", len);
    }

    // printf("cleaning up\n"); // just for debug
    fclose(fp);
    close(send_sock);
    return 0;
}

void error_handling(const char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}