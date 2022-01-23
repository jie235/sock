#include <cstdio>
#include <cstdlib>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>

void read_childproc(int sig) {
    int status;
    pid_t pid = waitpid(-1, &status, WNOHANG);
    if (WIFEXITED(status)) {
        printf("Remove proc id: %d\n", pid);
        printf("child send: %d\n", WEXITSTATUS(status));
    }
}

int main(int argc, char *argv[]) {
    struct sigaction act;
    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGCHLD, &act, 0);

    pid_t pid = fork();
    if (pid == 0) {
        printf("Hi! I'm child process\n");
        sleep(10);
        return 10;
    } else {
        printf("Child process id: %d\n", pid);
        pid = fork();
        if (pid == 0) {
            // this child process will exit first cause it has a smaller
            // sleeping time
            printf("Hi! I'm another child process\n");
            sleep(3);
            exit(24);
        } else {
            printf("Child process id: %d\n", pid);
            for (int i = 0; i < 5; ++i) {
                printf("wait...\n");
                sleep(3);
            }
        }
    }
    return 0;
}