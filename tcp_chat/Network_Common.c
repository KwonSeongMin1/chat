#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "Network_Common.h"

int start_tcp_server(uint16_t nPort, int nBacklog){
	struct sockaddr_in stSAddr;
	int nSFd=-1;
	nSFd=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(nSFd<0){
		printf("Creating a socket was failed.\n");
		return -1;
	}

	bzero(&stSAddr, sizeof(stSAddr));
	stSAddr.sin_family=AF_INET;
	stSAddr.sin_addr.s_addr=htonl(INADDR_ANY);
	stSAddr.sin_port=htons(nPort);

	if(bind(nSFd,(struct sockaddr*)&stSAddr, sizeof(stSAddr))<0){
		printf("Socket Binding Failure.\n");
		close(nSFd);
		return -1;
	}
	if(listen(nSFd,nBacklog)!=0){
		printf("listening Failure.\n");
		close(nSFd);
		return -1;
	}

	return nSFd;
}

int connect_to_tcp_server(char* strIpAddress, uint16_t nPort){
	int nCFd=-1;
	struct sockaddr_in stCAddr;
	memset(&stCAddr,0,sizeof(stCAddr));
	nCFd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(nCFd<0){
		printf("Creating a socket was failed.\n");
		return -1;
	}
	stCAddr.sin_family=AF_INET;
	stCAddr.sin_addr.s_addr=(in_addr_t)inet_addr(strIpAddress);
	stCAddr.sin_port=htons(nPort);
	if(connect(nCFd,(struct sockaddr*)&stCAddr,sizeof(stCAddr))==0){
		printf("Connection Success.\n");
		}
	else{
		printf("Connection failure.\n");
		close(nCFd);
		nCFd=-1;
		}
	return nCFd;
}

