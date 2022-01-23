#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>

void error_handling(const char *);

constexpr ushort port = 9091;
constexpr int buff_size = 0x01 << 8;
constexpr const char *multicast_addr = "239.1.1.2";

int main(int argc, char *argv[]) {
    sockaddr_in from_addr;
    int recv_sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (recv_sock == -1) {
        error_handling("socket() error");
    }
    memset(&from_addr, 0, sizeof(from_addr));
    from_addr.sin_family = AF_INET;
    from_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    from_addr.sin_port = htons(port);

    if (bind(recv_sock, reinterpret_cast<sockaddr *>(&from_addr),
             sizeof(from_addr)) == -1) {
        error_handling("bind() error");
    }

    printf("praparing multicast group \n"); // used for debug

    struct ip_mreq join_addr;
    join_addr.imr_multiaddr.s_addr = inet_addr(multicast_addr);
    // host address of joined socket
    join_addr.imr_interface.s_addr = htonl(INADDR_ANY);
    setsockopt(recv_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &join_addr,
               sizeof(join_addr));

    printf("praparing set time-await.\n");
    int option = true;
    socklen_t optlen = sizeof(option);
    setsockopt(recv_sock, SOL_SOCKET, SO_REUSEADDR, &option, optlen);

    printf("praparing outputing:\n");
    // DONE: reading data from multicast group is ok. but on visual maching, it
    // sometimes doesn't work(vmware+centos8)
    char message[buff_size];
    while (true) {
        // don't need to know where the message comes from
        int str_len = recvfrom(recv_sock, message, buff_size - 1, 0, NULL, 0);
        // break if meets an error
        if (str_len < 0) {
            break;
        }
        message[str_len] = 0;
        fputs(message, stdout);
    }
    printf("cleaning up");
    close(recv_sock);
    return 0;
}

void error_handling(const char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}