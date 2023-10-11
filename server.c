#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_len = sizeof(client_address);

    // 서버 소켓 생성
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 서버 주소 설정
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080); // 포트 번호 8080 사용
    server_address.sin_addr.s_addr = INADDR_ANY;

    // 서버 소켓에 주소 바인딩
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }

    // 서버가 클라이언트 연결을 기다림
    if (listen(server_socket, 5) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port 8080...\n");

    // 클라이언트 연결 대기
    client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len);
    if (client_socket < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    char buffer[1024];
    int n;

    // 클라이언트로부터 데이터를 받아서 에코
    while (1) {
        n = read(client_socket, buffer, sizeof(buffer));
        if (n <= 0) {
            perror("Read error");
            break;
        }

        write(client_socket, buffer, n); // 클라이언트에 에코
    }

    // 소켓 닫기
    close(client_socket);
    close(server_socket);

    return 0;
}

