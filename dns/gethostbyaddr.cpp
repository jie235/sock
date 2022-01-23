#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

void error_handling(const char *);

int main(int argc, char *argv[]) {
    hostent *host;
    sockaddr_in srv_addr;
    memset(&srv_addr, 0, sizeof(srv_addr));
    srv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    host = gethostbyaddr(&srv_addr.sin_addr, 4, AF_INET);

    if (!host) {
        error_handling("gethostbyaddr() error");
    }
    printf("Official name: %s\n", host->h_name);
    for (int i = 0; host->h_aliases[i]; ++i) {
        printf("Alias %d %s\n", i + 1, host->h_aliases[i]);
    }
    printf("Address type: %s\n",
           host->h_addrtype == AF_INET ? "AF_INET" : "AF_INET6");
    for (int i = 0; host->h_addr_list[i]; ++i) {
        printf("Address %d %s", i + 1,
               inet_ntoa(*reinterpret_cast<in_addr *>(host->h_addr_list[i])));
    }
    return 0;
}

void error_handling(const char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}