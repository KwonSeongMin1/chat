#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "network.h"

int main(int argc, char *argv[]) {

	if(argc!=3){
		printf("Usage : %s <IP> <Port>\n",argv[0]);
		exit(1);
	}

    int client_fd;
    struct sockaddr_in server_addr;

    // Create socket
	client_fd=connect_server(argv[1],atoi(argv[2]));
    printf("Connected to server on port %d\n", PORT);

    char message[1024];
    while (1) {
        printf("Enter a message (or 'exit' to quit): ");
        fgets(message, sizeof(message), stdin);

        if (send(client_fd, message, strlen(message), 0) == -1) {
            perror("Send failed");
            break;
        }

        if (strncmp(message, "exit", 4) == 0) {
            printf("Closing connection...\n");
            break;
        }
    }

    close(client_fd);
    return 0;
}

