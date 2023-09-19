#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include "network.h"


#define MAX_CLIENTS 10
#define MAX_EVENTS 10
#define DEFAULT_PORT 8080


int main(int argc, char *argv[]) {

	// option variable
	int opt;
	char *port = NULL;

	// network variable
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

	while((opt=getopt(argc,argv, "hp:"))!=-1){
		switch(opt){
			case 'h':
				printf("\t -p <port> (default : 8080)\n");
				exit(1);
			case 'p':
				if(optarg){
					port = optarg;
				}
				else{
					fprintf(stderr, "Option -p requires an argument.\n");
					exit(EXIT_FAILURE);
				}
				break;
			default:
				printf("invalid option : \'%c\'\nTry to use option \'-h\'\n",opt);
				exit(1);
		}
	}

	// if didn't use opt port
	if(port == NULL){
		server_fd = start_server(DEFAULT_PORT, MAX_CLIENTS);
	}
	else{
		server_fd = start_server(atoi(port), MAX_CLIENTS);
	}


    // create epoll instance
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("Epoll creation failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

	// epoll variable
    struct epoll_event ev, events[MAX_EVENTS];
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;

    // Add server socket to epoll
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) == -1) {
        perror("Epoll control failed");
        close(server_fd);
        close(epoll_fd);
        exit(EXIT_FAILURE);
    }
	

	// create server & server info
	printf("start server.");
//	printf("port : %d\n",atoi(port));

	// server info



    while (1) {
        int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (num_events == -1) {
            perror("Epoll wait failed");
            break;
        }

        for (int i = 0; i < num_events; i++) {
            int fd = events[i].data.fd;

            if (fd == server_fd) {
                // Accept new client connection
                client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
                if (client_fd == -1) {
                    perror("Accepting client failed");
                    continue;
                }

                printf("New client connected: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

                ev.events = EPOLLIN | EPOLLET; // Edge-triggered
                ev.data.fd = client_fd;

                // Add client socket to epoll
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1) {
                    perror("Epoll control failed");
                    close(client_fd);
                    continue;
                }
            } else {
                // Handle client data
                char buffer[1024];
                int bytes = recv(fd, buffer, sizeof(buffer), 0);
                if (bytes <= 0) {
                    // Client disconnected
                    printf("Client disconnected\n");
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                } else {
                    // Process received data
                    buffer[bytes] = '\0';
                    printf("Received from client (%d): %s", fd, buffer);
                    // You can send a response back to the client here if needed.
                }
            }
        }
    }

    close(server_fd);
    close(epoll_fd);
    return 0;
}

