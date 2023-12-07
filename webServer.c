#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MAX_BUFFER_SIZE 1024
#define MAX_METHOD_SIZE 10
#define MAX_URI_SIZE 100
#define MAX_CGI_OUTPUT 4096

// GET 요청을 처리하는 함수
void handle_get_request(int client_socket, const char *uri);

// POST 요청을 처리하는 함수
void handle_post_request(int client_socket, const char *uri, const char *content);

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_len = sizeof(client_address);
    char buffer[MAX_BUFFER_SIZE];

    // 서버 소켓 생성
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("소켓 생성 실패");
        exit(EXIT_FAILURE);
    }

    // 서버 주소 구조체 초기화
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(8080);

    // 서버 소켓에 주소 할당
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("바인딩 실패");
        exit(EXIT_FAILURE);
    }

    // 클라이언트로부터의 연결 대기
    if (listen(server_socket, 5) == -1) {
        perror("리스닝 실패");
        exit(EXIT_FAILURE);
    }

    printf("포트 8080에서 대기 중...\n");

    while (1) {
        // 클라이언트로부터의 연결 수락
        if ((client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len)) == -1) {
            perror("연결 수락 실패");
            exit(EXIT_FAILURE);
        }

        // 클라이언트로부터의 요청 수신
        recv(client_socket, buffer, sizeof(buffer), 0);

        // 요청 파싱
        char method[MAX_METHOD_SIZE], uri[MAX_URI_SIZE];
        sscanf(buffer, "%s %s", method, uri);

        // GET 또는 POST 요청 처리
        if (strcmp(method, "GET") == 0) {
            handle_get_request(client_socket, uri);
        } else if (strcmp(method, "POST") == 0) {
            // Content-Length 헤더에서 컨텐츠 길이 찾기
            char *content_length_str = strstr(buffer, "Content-Length:");
            if (content_length_str != NULL) {
                int content_length;
                sscanf(content_length_str, "Content-Length: %d", &content_length);

                // 컨텐츠 읽기
                char content[MAX_BUFFER_SIZE];
                recv(client_socket, content, content_length, 0);

                handle_post_request(client_socket, uri, content);
            } else {
                // Content-Length 헤더를 찾지 못한 경우
                // 적절한 처리 수행 (예: 400 Bad Request 반환)
                printf("400 Bad Request\n");
            }
        }

        // 클라이언트 소켓 닫기
        close(client_socket);
    }

    // 서버 소켓 닫기
    close(server_socket);

    return 0;
}

// GET 요청 처리
void handle_get_request(int client_socket, const char *uri) {
    // GET 요청에 대한 간단한 응답 생성
    char response[MAX_BUFFER_SIZE];
    snprintf(response, sizeof(response),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/html\r\n"
             "\r\n"
             "<html><body><h1>Hello,%s!</h1></body></html>",
             uri);

    // 응답을 클라이언트에게 전송
    send(client_socket, response, strlen(response), 0);
}

// POST 요청 처리
void handle_post_request(int client_socket, const char *uri, const char *content) {
    // POST 요청에 대한 간단한 응답 생성
    char response[MAX_BUFFER_SIZE];
    snprintf(response, sizeof(response),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/html\r\n"
             "\r\n"
             "<html><body><h1>Hello, %s!</h1><p>Content: %s</p></body></html>",
             uri, content);

    // 응답을 클라이언트에게 전송
    send(client_socket, response, strlen(response), 0);
}
