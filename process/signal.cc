#include <csignal>
#include <cstdio>
#include <signal.h>
#include <unistd.h>

void timeout(int sig) {
    if (sig == SIGALRM) {
        puts("Time out!");
    }
    // register a new SIGALRM signal
    alarm(2);
}

void keycontrol(int sig) {
    if (sig == SIGINT) {
        puts("Ctrl + C pressed");
    }
}

int main(int argc, char *argv[]) {
    signal(SIGALRM, timeout);
    signal(SIGINT, keycontrol);
    // generate a signal in 2 seconds
    alarm(2);

    for (int i = 0; i < 3; ++i) {
        puts("wait...");
        sleep(100);
    }

    return 0;
}