#include <cstdio>
#include <cstdlib>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int status;
    pid_t pid = fork();

    // child process
    if (pid == 0) {
        // let first child process sleep, then the parent will first catch 7
        sleep(20);
        return 3;
    } else {
        printf("Child pid: %d\n", pid);
        pid = fork();
        if (pid == 0) {
            exit(7);
        } else {
            printf("Child pid: %d\n", pid);

            // which child process reach here first then 'wait' catch whom first
            wait(&status);
            if (WIFEXITED(status)) {
                printf("Child send one: %d\n", WEXITSTATUS(status));
            }
            wait(&status);
            if (WIFEXITED(status)) {
                printf("Child send two: %d\n", WEXITSTATUS(status));
            }

            // to inspect child process' status, let parent process sleep
            sleep(30);
        }
    }
    return 0;
}