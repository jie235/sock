#include <bits/types/FILE.h>
#include <cstddef>
#include <cstdio>
#include <cstdlib>

int main(int argc, char *argv[]) {
    FILE *fp = fopen("./news.txt", "rt");
    if (fp == NULL) {
        printf("error");
    } else {
        printf("success");
    }
    return 0;
}