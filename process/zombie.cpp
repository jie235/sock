#include <arpa/inet.h>
#include <cstdio>
#include <cstring>
#include <sched.h>
#include <sys/socket.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    pid_t pid = fork();
    if (pid == 0) {
        puts("Hi, I'm child process.");
    } else {
        printf("Child process ID: %d \n", pid);
        sleep(30);
    }

    if (pid == 0) {
        // instructions in child process will not wait parant process, in this
        // context, the following sentenses will output first, then wait for
        // parent awake
        puts("End child process");
    } else {
        puts("End parent process");
    }

    return 0;
}