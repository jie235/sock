#include <cstdio>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>

constexpr int buff_size = 30;

int main(int argc, char *argv[]) {
    fd_set reads, cpy_reads;
    int result, str_len;
    char buff[buff_size];
    struct timeval timeout;

    FD_ZERO(&reads);
    // 0 refers to stdin
    FD_SET(0, &reads);

    while (true) {
        // after every call of select(), all value in fd_set will be set as 0
        // excepting changed fd so if u want to rememver the initial values in
        // fd_set, don't forget to do this copy
        cpy_reads = reads;
        // every time before calling select() function, you need to reset
        // timeval as a certain value cause after calling select() function
        // timeval variable's value will be replace by remaining time to preset
        // timeout
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        result = select(1, &cpy_reads, 0, 0, &timeout);
        if (result == -1) {
            puts("select() error!");
            break;
        } else if (result == 0) {
            puts("Time-out!");
        } else {
            if (FD_ISSET(0, &cpy_reads)) {
                str_len = read(0, buff, buff_size);
                buff[str_len] = 0;
                printf("message from console: %s", buff);
            }
        }
    }
    return 0;
}