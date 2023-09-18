#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/epoll.h>

#define MAX_EVENTS 10

int main(){

	int epoll_fd, nfds;
	struct epoll_event ev, events[MAX_EVENTS];

	epoll_fd = epoll_create1(0);
	if(epoll_fd==-1){
		perror("epoll_create1");
		exit(EXIT_FAILURE);
	}

	ev.events=EPOLLIN;
	ev.data.fd=0;
	if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD,0,&ev)==-1){
		perror("epoll_ctl");
		exit(EXIT_FAILURE);
	}

	printf("Type something : \n");

	while(1){
		nfds=epoll_wait(epoll_fd,events,MAX_EVENTS,-1);
		if(nfds==-1){
			perror("epoll_wait");
			exit(EXIT_FAILURE);
		}

		for(int i=0;i<nfds;++i){
			if(events[i].data.fd==0){
				char buffer[256];
				ssize_t bytes=read(0,buffer,sizeof(buffer));
				if(bytes<=0){
					perror("read");
					exit(EXIT_FAILURE);
				}
				buffer[bytes-1]='\0';
				printf("Typed : %s\n",buffer);
			}
		}
	}

	return 0;
}

