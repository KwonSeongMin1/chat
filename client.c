#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

int main() {
    int client_socket;
    struct sockaddr_in server_address;

    // 클라이언트 소켓 생성
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 서버 주소 설정
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080); // 서버 포트 번호
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1"); // 서버 IP 주소

    // 서버에 연결
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    char message[1024];
    int n;

    // 사용자 입력 받아서 서버에 전송
    while (1) {
        printf("Enter a message: ");
        fgets(message, sizeof(message), stdin);
        n = write(client_socket, message, strlen(message));

        if (n < 0) {
            perror("Write error");
            break;
        }

        n = read(client_socket, message, sizeof(message));
        if (n <= 0) {
            perror("Read error");
            break;
        }

        message[n] = '\0';
        printf("Server: %s", message);
    }

    // 소켓 닫기
    close(client_socket);

    return 0;
}

