#include <cstdio>
#include <unistd.h>

constexpr int buff_size = 0x01 << 10;

int main(int argc, char *argv[]) {
    int fds1[2], fds2[2];
    pid_t pid;
    char str1[] = "Who are you?";
    char str2[] = "Thank you for your message";
    char message[buff_size];

    pipe(fds1);
    pipe(fds2);
    pid = fork();
    if (pid == 0) {
        write(fds1[1], str1, sizeof(str1));
        read(fds2[0], message, buff_size);
        printf("Message from parent: %s\n", message);
    } else {
        read(fds1[0], message, buff_size);
        printf("Message form child: %s\n", message);
        write(fds2[1], str2, sizeof(str2));
        sleep(3);
    }
    return 0;
}