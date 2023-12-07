#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 5

// 공유 데이터 구조체
typedef struct {
    int buffer[BUFFER_SIZE];
    int in;  // 생산자가 데이터를 넣을 위치
    int out; // 소비자가 데이터를 가져올 위치
    sem_t empty; // 빈 공간의 개수를 나타내는 세마포어
    sem_t full;  // 찬 공간의 개수를 나타내는 세마포어
    pthread_mutex_t mutex; // 임계 구역에 진입하기 위한 뮤텍스
} BoundedBuffer;

BoundedBuffer buffer; // 공유 데이터

// 생산자 함수
void *producer(void *arg) {
    int item;
    while (1) {
        item = rand() % 100; // 임의의 아이템 생성
        sem_wait(&buffer.empty);
        pthread_mutex_lock(&buffer.mutex);

        // 버퍼에 아이템 추가
        buffer.buffer[buffer.in] = item;
        printf("Produced: %d\n", item);
        buffer.in = (buffer.in + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&buffer.mutex);
        sem_post(&buffer.full);
        sleep(rand() % 3); // 생산 시간 간격 조절
    }
}

// 소비자 함수
void *consumer(void *arg) {
    int item;
    while (1) {
        sem_wait(&buffer.full);
        pthread_mutex_lock(&buffer.mutex);

        // 버퍼에서 아이템 가져오기
        item = buffer.buffer[buffer.out];
        printf("Consumed: %d\n", item);
        buffer.out = (buffer.out + 1) % BUFFER_SIZE;

        pthread_mutex_unlock(&buffer.mutex);
        sem_post(&buffer.empty);
        sleep(rand() % 3); // 소비 시간 간격 조절
    }
}

int main() {
    // 초기화
    buffer.in = 0;
    buffer.out = 0;
    sem_init(&buffer.empty, 0, BUFFER_SIZE);
    sem_init(&buffer.full, 0, 0);
    pthread_mutex_init(&buffer.mutex, NULL);

    // 생산자와 소비자 스레드 생성
    pthread_t producer_thread, consumer_thread;
    pthread_create(&producer_thread, NULL, producer, NULL);
    pthread_create(&consumer_thread, NULL, consumer, NULL);

    // 메인 스레드에서 생산자와 소비자 스레드 종료를 대기
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    // 세마포어와 뮤텍스 해제
    sem_destroy(&buffer.empty);
    sem_destroy(&buffer.full);
    pthread_mutex_destroy(&buffer.mutex);

    return 0;
}
