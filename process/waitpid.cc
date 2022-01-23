#include <cstdio>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int status;
    pid_t pid = fork();
    if (pid == 0) {
        sleep(15);
        puts("sleep 15 seconds");
        return 24;
    } else {
        // if on child process exits, gets 0
        while (!waitpid(-1, &status, WNOHANG)) {
            sleep(3);
            puts("sleep 3 seconds");
        }
        if (WIFEXITED(status)) {
            printf("Child send %d\n", WEXITSTATUS(status));
        }
    }
    return 0;
}