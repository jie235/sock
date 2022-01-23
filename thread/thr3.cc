#include <cstdio>
#include <cstring>
#include <pthread.h>
#include <unistd.h>

void *thread_summation(void *);

int sum = 0;

// this program has bug
int main(int arg, char *argv[]) {
    pthread_t t_id1, t_id2;
    int range1[] = {1, 5};
    int range2[] = {6, 10};
    pthread_create(&t_id1, nullptr, thread_summation,
                   reinterpret_cast<void *>(range1));
    pthread_create(&t_id2, nullptr, thread_summation,
                   reinterpret_cast<void *>(range2));

    pthread_join(t_id1, nullptr);
    pthread_join(t_id2, nullptr);
    printf("result: %d\n", sum);
    return 0;
}

void *thread_summation(void *arg) {
    int start = reinterpret_cast<int *>(arg)[0];
    int end = reinterpret_cast<int *>(arg)[1];
    while (start <= end) {
        sum += start;
        start++;
    }
    return nullptr;
}