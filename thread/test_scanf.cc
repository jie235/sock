#include <cstdio>

int main(int argc, char *argv[]) {
    int num;
    for (int i = 0; i < 5; ++i) {
        printf("input num: ");
        //如果scanf未读取到匹配格式的内容，将引起stdin阻塞，但是不会丢弃stdin中的内容。如果想丢弃stdin中的内容，用fflush(stdin)，这样的话整个scanf读取到stdin但未正确匹配格式的内容都会被丢弃
        scanf("%d", &num);
    }
    return 0;
}