#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>

void errorHandling(const char*);

// constexpr const char* host = "127.0.0.1";
// constexpr ushort port = 9091;

int main(int argc, char* argv[]){
    struct sockaddr_in srv_addr;
    char message[30];

    char* host = argv[1];
    ushort port = std::stoi(argv[2]);
    
    auto sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sock == -1){
        errorHandling("socket() error.");
    }

    memset(&srv_addr, 0, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    inet_aton(host, &srv_addr.sin_addr);
    srv_addr.sin_port = htons(port);

    //(struct sockaddr*)&srv_addr
    if(connect(sock, reinterpret_cast<struct sockaddr*>(&srv_addr), sizeof(srv_addr)) == -1){
        errorHandling("connect() error.");
    }

    auto str_len = read(sock, message, sizeof(message)-1);
    if(str_len == -1){
        errorHandling("read() error.");
    }

    printf("message from server: %s \n", message);
    close(sock);
    return 0;
}

void errorHandling(const char* message){
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}