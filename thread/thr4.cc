#include <cstdio>
#include <cstring>
#include <pthread.h>
#include <unistd.h>

void *thread_inc(void *);
void *thread_dec(void *);
long long sum = 0;
constexpr int THREAD_NUM = 5000;

// this program has bug
int main(int argc, char *argv[]) {
    pthread_t thread_id[THREAD_NUM];
    for (int i = 0; i < THREAD_NUM; ++i) {
        if (i % 2 == 0) {
            pthread_create(&thread_id[i], nullptr, thread_inc, nullptr);
        } else {
            pthread_create(&thread_id[i], nullptr, thread_dec, nullptr);
        }
    }
    for (int i = 0; i < THREAD_NUM; ++i) {
        pthread_join(thread_id[i], nullptr);
    }
    printf("result: %lld\n", sum);
    return 0;
}

void *thread_inc(void *arg) {
    for (int i = 0; i < 500000; ++i) {
        sum++;
    }
    return nullptr;
}

void *thread_dec(void *arg) {
    for (int i = 0; i < 500000; ++i) {
        sum--;
    }
    return nullptr;
}