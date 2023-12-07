#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MAX_CLIENTS 10
#define MAX_BUFFER_SIZE 1024

int main() {
    int server_socket, client_sockets[MAX_CLIENTS];
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_len = sizeof(client_address);
    fd_set readfds;
    char buffer[MAX_BUFFER_SIZE];

    // 소켓 서버 생성
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 서버 주소 구조체 초기화
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(8080);

    // 서버 소켓 바인딩
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // 수신 중인 연결에 대해 듣기
    if (listen(server_socket, 5) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port 8080...\n");

    // 클라이언트 소켓 배열 초기화
    for (int i = 0; i < MAX_CLIENTS; ++i) {
        client_sockets[i] = 0;
    }

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(server_socket, &readfds);

        // 모든 유효한 클라이언트 소켓을 세트에 추가
        for (int i = 0; i < MAX_CLIENTS; ++i) {
            if (client_sockets[i] > 0) {
                FD_SET(client_sockets[i], &readfds);
            }
        }

        // 소켓에서 활동 대기
        if (select(FD_SETSIZE, &readfds, NULL, NULL, NULL) == -1) {
            perror("Select failed");
            exit(EXIT_FAILURE);
        }

        // 수신 연결 확인
        if (FD_ISSET(server_socket, &readfds)) {
            int new_socket;

            // 수신 연결 수락
            if ((new_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len)) == -1) {
                perror("Accept failed");
                exit(EXIT_FAILURE);
            }

            // 클라이언트 소켓 배열에 새 연결 추가
            for (int i = 0; i < MAX_CLIENTS; ++i) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = new_socket;
                    printf("New connection, socket fd is %d, ip is: %s, port: %d\n",
                           new_socket, inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
                    break;
                }
            }
        }

        // 클라이언트의 데이터 확인
        for (int i = 0; i < MAX_CLIENTS; ++i) {
            int client_socket = client_sockets[i];
            if (FD_ISSET(client_socket, &readfds)) {
                // 클라이언트에서 데이터 읽기
                int read_size = recv(client_socket, buffer, sizeof(buffer), 0);
                if (read_size <= 0) {
                    // 연결이 닫힘 또는 오류
                    close(client_socket);
                    client_sockets[i] = 0;
                    printf("Client disconnected, socket fd is %d\n", client_socket);
                } else {
                    // 수신한 메시지를 다른 모든 클라이언트에 브로드캐스트
                    for (int j = 0; j < MAX_CLIENTS; ++j) {
                        if (client_sockets[j] > 0 && client_sockets[j] != client_socket) {
                            send(client_sockets[j], buffer, read_size, 0);
                        }
                    }
                }
            }
        }
    }

    return 0;
}
