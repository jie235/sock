#include <arpa/inet.h>
#include <bits/types/FILE.h>
#include <csignal>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void error_handling(const char *);
void read_childproc(int);

constexpr ushort port = 9091;
constexpr int queue_size = 5;
constexpr int buff_size = 0x01 << 10;

int main(int argc, char *argv[]) {
    sockaddr_in srv_addr, clnt_addr;
    int srv_sock, clnt_sock;
    int fds[2];

    srv_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (srv_sock == -1) {
        error_handling("socket() error");
    }
    memset(&srv_addr, 0, sizeof(srv_addr));
    srv_addr.sin_family = AF_INET;
    srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    srv_addr.sin_port = htons(port);
    if (bind(srv_sock, reinterpret_cast<sockaddr *>(&srv_addr),
             sizeof(srv_addr)) == -1) {
        error_handling("bind() error");
    }
    if (listen(srv_sock, queue_size) == -1) {
        error_handling("listen() error");
    }

    // register event listener
    struct sigaction act;
    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    pipe(fds);
    pid_t pid = fork();
    if (pid == 0) {
        // write data to file
        //"t" means open with text mode(default binary)
        FILE *fp = fopen("./clientData.txt", "wt");
        size_t str_len;
        char message[buff_size];

        // FIXME
        //  following code(for loop) goes well and .txt file gets data but in
        //  do-while loop or while loop it will get nothing

        // read from pipe, here if write in a while loop, .txt file will get
        // nothing
        for (int i = 0; i < 5; ++i) {
            str_len = read(fds[0], message, buff_size);
            fwrite(message, 1, str_len, fp);
        }

        // TODO
        // debug multi process program
        //  do {
        //      str_len = read(fds[0], message, buff_size);
        //      // use file operation function to operate file, not system
        //      general
        //      // function
        //      fwrite(message, 1, str_len, fp);
        //  } while (str_len != 0);

        fclose(fp);
        return 0;
    }

    socklen_t clnt_addr_len;
    while (true) {
        clnt_addr_len = sizeof(clnt_addr);
        clnt_sock = accept(srv_sock, reinterpret_cast<sockaddr *>(&clnt_addr),
                           &clnt_addr_len);
        if (clnt_sock == -1) {
            continue;
        } else {
            printf("new client connected\n");
        }
        pid = fork();
        if (pid == 0) {
            close(srv_sock);
            char message[buff_size];
            ssize_t str_len;
            while ((str_len = read(clnt_sock, message, buff_size)) != 0) {
                write(clnt_sock, message, str_len);
                write(fds[1], message, str_len);
            }
            close(clnt_sock);
            printf("client disconnected...\n");
            return 0;
        } else {
            close(clnt_sock);
        }
    }
    close(srv_sock);
    return 0;
}

void read_childproc(int sig) {
    int status;
    pid_t pid = waitpid(SIGCHLD, &status, WNOHANG);
    if (WIFEXITED(status)) {
        printf("child process end: %d\n", pid);
        printf("child process send: %d\n", WEXITSTATUS(status));
    }
}

void error_handling(const char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(EXIT_FAILURE);
}