#include <cstdio>
#include <sched.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int gval = 10, lval = 20;
    pid_t pid;
    gval++;
    lval += 5;

    pid = fork();
    if (pid == 0) {
        gval += 2;
        lval += 2;
    } else {
        gval -= 2;
        lval -= 2;
    }

    if (pid == 0) {
        printf("Child process: [%d %d]\n", gval, lval);
    } else {
        printf("Parent process: [%d %d]\n", gval, lval);
    }
    return 0;
}