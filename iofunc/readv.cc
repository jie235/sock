#include <bits/types/struct_iovec.h>
#include <cstdio>
#include <cstring>
#include <sys/socket.h>
#include <sys/uio.h>

int main(int argc, char *argv[]) {
    int buff_size = 0x01 << 8;
    char buf1[buff_size];
    char buf2[buff_size];

    struct iovec iov[2];
    iov[0].iov_base = buf1;
    iov[0].iov_len = 5;
    iov[1].iov_base = buf2;
    iov[1].iov_len = buff_size;

    int str_len = readv(0, iov, 2);
    printf("\nRead bytes: %d", str_len);
    printf("\nFirst message: %s", buf1);
    printf("\nSecond message: %s\n", buf2);
    return 0;
}