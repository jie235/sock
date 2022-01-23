#include <cstdio>
#include <unistd.h>

constexpr int buff_size = 0x01 << 10;

int main(int argc, char *argv[]) {
    int fds[2];
    char str1[] = "Who are you?";
    char str2[] = "Thank you for your message";
    char buff[buff_size];
    pid_t pid;

    pipe(fds);
    pid = fork();
    if (pid == 0) {
        write(fds[1], str1, sizeof(str1));
        // do not read immediately, or will get what it wrote
        sleep(2);
        read(fds[0], buff, buff_size);
        printf("Child proc output: %s \n", buff);
    } else {
        read(fds[0], buff, buff_size);
        printf("Parent proc output: %s\n", buff);
        write(fds[1], str2, sizeof(str2));
        // prevent parant process exiting(if so, child process will be
        // destroyed)
        sleep(3);
    }
    return 0;
}