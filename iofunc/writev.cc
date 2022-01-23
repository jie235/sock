#include <bits/types/struct_iovec.h>
#include <cstdio>
#include <cstring>
#include <sys/uio.h>

int main(int argc, char *argv[]) {
    char buf1[] = "ABCDEFG";
    char buf2[] = "1234567";

    int str_len;
    struct iovec iov[2];
    iov[0].iov_base = buf1;
    iov[0].iov_len = strlen(buf1);
    iov[1].iov_base = buf2;
    iov[1].iov_len = strlen(buf2);

    str_len = writev(1, iov, 2);
    printf("\nWrite bytes: %d\n", str_len);
    return 0;
}