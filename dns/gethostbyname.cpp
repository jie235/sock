#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

void error_handling(const char *);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        error_handling("Address error");
    }

    const char *address = argv[1];
    hostent *host;

    host = gethostbyname(address);
    if (!host) {
        error_handling("gethostbyname() error");
    }

    printf("Official name: %s", host->h_name);
    printf("Alias list: \n");
    for (int i = 0; host->h_aliases[i]; ++i) {
        printf("%s", host->h_aliases[i]);
    }
    printf("Address type: %s\n",
           host->h_addrtype == AF_INET ? "AF_INET" : "AF_INET6");
    printf("Address list: \n");
    for (int i = 0; host->h_addr_list[i]; ++i) {
        // 要进行地址转换
        printf("Ip addr: %d %s\n", i + 1,
               inet_ntoa(*(reinterpret_cast<in_addr *>(host->h_addr_list[i]))));
    }

    return 0;
}

void error_handling(const char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}