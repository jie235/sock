#include <cstdio>
#include <pthread.h>
#include <unistd.h>

void *thread_inc(void *);
void *thread_dec(void *);
long long sum = 0;
constexpr int THREAD_NUM = 5000;
pthread_mutex_t mutex;

int main(int argc, char *argv[]) {
    pthread_t thread_id[THREAD_NUM];
    pthread_mutex_init(&mutex, nullptr);
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
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < 50000; ++i) {
        sum++;
    }
    pthread_mutex_unlock(&mutex);
    return nullptr;
}

void *thread_dec(void *arg) {
    for (int i = 0; i < 50000; ++i) {
        pthread_mutex_lock(&mutex);
        sum--;
        pthread_mutex_unlock(&mutex);
    }
    return nullptr;
}