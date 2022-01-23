#include <cstdio>
#include <netinet/in.h>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <cstdlib>
#include <arpa/inet.h>

void errorHandling(const char* message);

// constexpr ushort srv_port = 9091;
constexpr char message[]= "Hello World!";

int main(int argc, char* argv[]){

    ushort srv_port = std::stoi(argv[1]);

    struct sockaddr_in srvAddr, clntAddr;
    socklen_t clnt_sock_len;

    memset(&srvAddr, 0, sizeof(srvAddr));
    srvAddr.sin_family = AF_INET;
    srvAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    srvAddr.sin_port = htons(srv_port);

    auto srv_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(srv_sock == -1){
        errorHandling("socket() error.");
    }

    //(struct sockaddr*)&srvAddr
    if(-1== bind(srv_sock, reinterpret_cast<struct sockaddr*>(&srvAddr), sizeof(srvAddr))){
        errorHandling("bind() error.");
    }

    if(listen(srv_sock, 5) == -1){
        errorHandling("listen() error.");
    }

    clnt_sock_len = sizeof(clntAddr);
    //(struct sockaddr*)&clntAddr
    auto clnt_sock = accept(srv_sock, reinterpret_cast<struct sockaddr*>(&clntAddr) , &clnt_sock_len);
    if(clnt_sock == -1){
        errorHandling("accept() error.");
    }
    
    write(clnt_sock, message, sizeof(message));
    close(clnt_sock);
    close(srv_sock);
    return 0;
}

void errorHandling(const char* message){
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}