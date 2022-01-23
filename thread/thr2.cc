#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <pthread.h>
#include <unistd.h>

void *thread_main(void *);

int main(int argc, char *argv[]) {
    int thread_param = 5;
    pthread_t t_id;
    if (pthread_create(&t_id, nullptr, thread_main,
                       reinterpret_cast<void *>(&thread_param)) != 0) {
        puts("pthread_create() error");
        return -1;
    }

    void *thr_ret;
    // pthread_join, means wait
    if (pthread_join(t_id, &thr_ret) != 0) {
        puts("pthread_join() error");
        return -1;
    }
    printf("Thread return message: %s", reinterpret_cast<char *>(thr_ret));
    // why here free? cause using malloc to allocate
    free(thr_ret);
    return 0;
}

void *thread_main(void *arg) {
    int i;
    int cnt = *(reinterpret_cast<int *>(arg));
    char *message = reinterpret_cast<char *>(malloc(sizeof(char) * 50));
    strcpy(message, "Hello, I'm thread~ \n");
    for (i = 0; i < cnt; ++i) {
        sleep(1);
        puts("running thread");
    }
    return message;
}