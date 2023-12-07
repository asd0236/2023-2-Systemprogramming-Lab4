#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_CLIENTS 5

// 공유 데이터 구조체
typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int newMessage;
    char message[256];
} SharedData;

SharedData sharedData;

// 클라이언트 쓰레드 함수
void *clientThread(void *arg) {
    int clientID = *((int *)arg);
    
    while (1) {
        // 클라이언트가 새로운 메시지를 요청할 때까지 대기
        pthread_mutex_lock(&sharedData.mutex);
        while (!sharedData.newMessage) {
            pthread_cond_wait(&sharedData.cond, &sharedData.mutex);
        }

        // 메시지 출력
        printf("Client %d received message: %s\n", clientID, sharedData.message);

        // 메시지 처리 후 초기화
        sharedData.newMessage = 0;
        sharedData.message[0] = '\0';

        pthread_mutex_unlock(&sharedData.mutex);
    }

    return NULL;
}

// 서버 쓰레드 함수
void *serverThread(void *arg) {
    int clientThreads[MAX_CLIENTS];
    pthread_t threads[MAX_CLIENTS];

    // 클라이언트 쓰레드 생성
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        clientThreads[i] = i;
        pthread_create(&threads[i], NULL, clientThread, (void *)&clientThreads[i]);
    }

    while (1) {
        // 서버가 새로운 메시지를 생성
        pthread_mutex_lock(&sharedData.mutex);

        // 메시지 생성 및 플래그 설정
        sprintf(sharedData.message, "Broadcast from Server!");
        sharedData.newMessage = 1;

        // 모든 클라이언트 쓰레드에게 메시지 전송
        pthread_cond_broadcast(&sharedData.cond);

        pthread_mutex_unlock(&sharedData.mutex);

        // 일정 시간마다 메시지 생성
        sleep(3);
    }

    // 클라이언트 쓰레드 종료
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        pthread_join(threads[i], NULL);
    }

    return NULL;
}

int main() {
    // 초기화
    sharedData.newMessage = 0;
    sharedData.message[0] = '\0';
    pthread_mutex_init(&sharedData.mutex, NULL);
    pthread_cond_init(&sharedData.cond, NULL);

    pthread_t serverThreadID;
    pthread_create(&serverThreadID, NULL, serverThread, NULL);

    // 메인 스레드에서 서버 쓰레드 종료를 대기
    pthread_join(serverThreadID, NULL);

    // 뮤텍스와 조건 변수 해제
    pthread_mutex_destroy(&sharedData.mutex);
    pthread_cond_destroy(&sharedData.cond);

    return 0;
}
