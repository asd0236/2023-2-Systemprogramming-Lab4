#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MAX_BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_address;
    char buffer[MAX_BUFFER_SIZE];

    // 클라이언트 소켓 생성
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 서버 주소 구조체 초기화
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(8080);

    // 서버에 연결
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to the server\n");

    while (1) {
        // 사용자 입력
        printf("Enter a message: ");
        fgets(buffer, sizeof(buffer), stdin);

        // 서버에 메세지 전송
        send(client_socket, buffer, strlen(buffer), 0);

        // 서버의 응답 수신 및 출력
        recv(client_socket, buffer, sizeof(buffer), 0);
        printf("Server says: %s\n", buffer);
    }

    // 소켓 닫기
    close(client_socket);

    return 0;
}
