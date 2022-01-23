#include <arpa/inet.h>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <system_error>
#include <unistd.h>

void error_handling(const char *);

constexpr ushort port = 9091;
constexpr const char *address = "127.0.0.1";
constexpr int buff_size = 30;

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

    if (connect(clnt_sock, reinterpret_cast<sockaddr *>(&srv_addr),
                sizeof(srv_addr)) == -1) {
        error_handling("connect() error");
    }

    auto fp = fopen("./recvData.dat", "wb");
    char message[buff_size];
    int read_cnt;
    while ((read_cnt = read(clnt_sock, message, buff_size)) != 0) {
        fwrite(message, 1, read_cnt, fp);
    }
    fclose(fp);

    const char *reply = "Thank you!";
    shutdown(clnt_sock, SHUT_RD);
    write(clnt_sock, reply, strlen(reply));

    close(clnt_sock);
    return 0;
}

void error_handling(const char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}