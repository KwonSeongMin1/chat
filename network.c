#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "network.h"

int start_server(uint16_t port, int client){

	int server_fd;
	struct sockaddr_in server_addr;
	
	// create socket
	server_fd = socket(AF_INET,SOCK_STREAM,0);
	
	// socket error handling
	if(server_fd<0){
		printf("Socket creation failed.");
		exit(EXIT_FAILURE);
	}
	printf("Socket creation success.\n");

	// set server address
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family=AF_INET;
	server_addr.sin_addr.s_addr=INADDR_ANY;
	server_addr.sin_port=htons(port);

	// bind socket & error handling
	if(bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr))<0){
		printf("Binding failed.");
		close(server_fd);
		exit(EXIT_FAILURE);
	}
	printf("Server binding success.\n");

	// listen client
	if(listen(server_fd, client)==-1){
		printf("listening failed.");
		close(server_fd);
		exit(EXIT_FAILURE);
	}
	printf("Start listening...\n");
	printf("port : %d\n",port);
	return server_fd;
}

int connect_server(char *ip, uint16_t port){

	int client_fd;
	struct sockaddr_in client_addr;

	client_fd=socket(AF_INET,SOCK_STREAM,0);

	// socket error handling
	if(client_fd<0){
		printf("Socket creation failed.");
		exit(EXIT_FAILURE);
	}
	
	//set client address
	memset(&client_addr, 0, sizeof(client_addr));
	client_addr.sin_family=AF_INET;
	client_addr.sin_addr.s_addr=(in_addr_t)inet_addr(ip);
	client_addr.sin_port=htons(port);

	// connect to server & error handling
	if(connect(client_fd, (struct sockaddr*)&client_addr,sizeof(client_addr))==0){
		printf("Connection Success.\n");
	}
	else{
		printf("Connection failed.");
		close(client_fd);
		exit(EXIT_FAILURE);
	}
	return client_fd;
}

