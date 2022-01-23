#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

void *read(void *);
void *accu(void *);
sem_t sem0, sem1;
static int num;

int main(int argc, char *argv[]) {
    pthread_t t_id0, t_id1;
    sem_init(&sem0, 0, 0);
    sem_init(&sem1, 0, 1);
    pthread_create(&t_id0, nullptr, read, nullptr);
    pthread_create(&t_id1, nullptr, accu, nullptr);
    pthread_join(t_id0, nullptr);
    pthread_join(t_id1, nullptr);
    sem_destroy(&sem0);
    sem_destroy(&sem1);
    return 0;
}

void *read(void *arg) {
    for (int i = 0; i < 5; ++i) {
        fputs("Input num: ", stdout);
        //第一次开始读之前，不允许写，所以应该让写操作的信号初始值为0
        sem_wait(
            &sem1); //让其他的读操作等待 --> 此信号对应允许同时进行的读操作数
        scanf("%d", &num);
        sem_post(&sem0); //读完之后允许写操作，而且只允许写操作
    }
    return nullptr;
}

void *accu(void *arg) {
    int sum = 0;
    for (int i = 0; i < 5; ++i) {
        //写之前设置信号，不允许读，防止读取的数据覆盖写入的数据
        sem_wait(&sem0); //--> 此操作对应同时允许写的操作数
        sum += num;      //这里加的是num, 不是i
        sem_post(&sem1); //--> 写完之后可以读，而且也只能读，所以释放读的信号
    }
    printf("Result: %d", sum);
    return nullptr;
}