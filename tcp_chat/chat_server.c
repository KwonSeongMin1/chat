#include <stdio.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <ifaddrs.h>
#include "Network_Common.h"
#define MAX_CLIENTS 10

int main(int argc, char *argv[]){
    struct sockaddr_in stCAddr;
	struct ifaddrs *ifaddr, *ifa;
    socklen_t nCAddr;
    struct pollfd rfds[MAX_CLIENTS+2];
    int nTimeout=0;
    int nRetval;
    int nKeepRunning=1;
	int current_client = 0;

	// client info
	int client_id[MAX_CLIENTS] = {0,};
	char nickname[MAX_CLIENTS][20];
	memset(nickname,0,sizeof(nickname));

	// Initialize FD
    for(int i=0;i<MAX_CLIENTS+2;i++){
        rfds[i].fd=-1;
        rfds[i].events=0;
        rfds[i].revents=0;
    }
	// error handling	
    if(argc!=3){
        printf("Usage : %s <port> <queue>\n",argv[0]);
        return -1;
    }

	// ip addr error handling
	if (getifaddrs(&ifaddr)==-1){
		perror("getifaddrs");
		exit(EXIT_FAILURE);
	}

	// server info
	printf("Start to tcp chat server...\n");

	for(ifa=ifaddr;ifa!=NULL;ifa=ifa->ifa_next){
		if(ifa->ifa_addr==NULL){
			continue;
		}

		if(ifa->ifa_addr->sa_family==AF_INET){
			struct sockaddr_in *addr = (struct sockaddr_in *)ifa->ifa_addr;
			char ip[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &(addr->sin_addr), ip, INET_ADDRSTRLEN);
			
			if(strcmp(ip, "127.0.0.1")==0){
				continue;
			}


			printf("Address : %s\n",ip);
		}
	}
	freeifaddrs(ifaddr);

    printf("Port : %s\n",argv[1]);
    printf("Queue : %s\n", argv[2]);
	printf("Max clients : %d\n",MAX_CLIENTS);

    if(atoi(argv[2])>MAX_CLIENTS){
		return -1;
	}

	// stdin >> fd[0]
    rfds[0].fd=0;
    rfds[0].events=POLLIN;
    rfds[0].revents=0;

	// create TCP server socket >>> fd[1]
    rfds[1].fd=start_tcp_server(atoi(argv[1]),atoi(argv[2]));
    if(rfds[1].fd<0){
        printf("Starting Server is failed.\n");
        return -1;
    }
    rfds[1].events=POLLIN;
    rfds[1].revents=0;
    nTimeout=1000;
    do{
		// poll... event monitering
        nRetval=poll(rfds,MAX_CLIENTS+2,nTimeout);
        if(nRetval>0){
            for(int i=0;i<MAX_CLIENTS+2;i++){
                if(rfds[i].fd<0) continue;
                if(rfds[i].revents&POLLIN){
                    if(i==0){
						// stdin 
                        char strBuffer[512];
                        int nBufferLen=0;
                        char strBuffer2[1024];
                        bzero(strBuffer,512);
                        bzero(strBuffer2,1024);
                        nBufferLen=read(0,strBuffer,512);
                        if(nBufferLen>0){
							// quit program
                            if(strncasecmp(strBuffer,"exit",4)==0){
                                nKeepRunning=0;
                                break;
                            }
							// client list
							if(strncasecmp(strBuffer,"/list",5)==0){
								printf("current clients : %d\n", current_client);
								for(int j=2;j<=current_client+1;j++){
									printf("fd : %d, nickname : %s\n",rfds[j].fd,nickname[j-2]);
								}
								break;
							}
							//
                            snprintf(strBuffer2,sizeof(strBuffer2),"\n\n[NOTICE] %s\n\n",strBuffer);
                            for(int i=2;i<MAX_CLIENTS+2;i++){
                                if(rfds[i].fd<0) continue;
                                send(rfds[i].fd,strBuffer2,sizeof(strBuffer2),0);
                            }
                        }
                    }
                    else if(i==1){
						// client connect
                        int fd=accept(rfds[i].fd,(struct sockaddr*)&stCAddr,&nCAddr);
                        if(fd>0){
                            for(int i=2;i<MAX_CLIENTS+2;i++){
                                if(rfds[i].fd<0){
                                    rfds[i].fd=fd;
                                    rfds[i].events=POLLIN;
                                    rfds[i].revents=0;
									
									// client (fd, nickname) -> server info
									char served_nickname[20];
									int nickname_len;
									bzero(served_nickname,20);
									nickname_len = read(rfds[i].fd, served_nickname, 20);

									printf("welcome!! nickname %s\n",served_nickname);
									client_id[current_client] = fd;
									strcpy(nickname[current_client], served_nickname);
									current_client++;
                                    break;
                                }
                            }
                        }
                    }
                    else{
						// client communication
                        char strBuffer[BUFSIZ];
                        int nBufferLen=0;
                        bzero(strBuffer,BUFSIZ);
                        nBufferLen=read(rfds[i].fd,strBuffer,BUFSIZ);
						char combined_message[512];

						// client disconnect
                        if(nBufferLen<=0){
                            close(rfds[i].fd);
                            rfds[i].fd=-1;
                            rfds[i].events=0;
                            rfds[i].revents=0;
							char closed_message[100];

							snprintf(closed_message, sizeof(closed_message), "%s is terminated.\n",nickname[i-2]);
							printf("%s is terminated.\n",nickname[i-2]);
							for(int i=2;i<MAX_CLIENTS+2;i++){
								send(rfds[i].fd,closed_message,sizeof(combined_message),0);
							}

							current_client -= 1;
                        }
                        else{

							char *served_nickname = strtok(strBuffer," ");
							char *whisper = strtok(NULL," ");
							// whisper
							int w = strncmp(whisper, "/w",2);

							// whisper detected
							if(!w){
								char *from = nickname[rfds[i].fd-4];
								char *to = strtok(NULL, " ");	// sended nickname
								char *message = strtok(NULL, " ");	// message

								//error handling
								if(to == NULL){
									for(int i=0;i<current_client;i++){
										if(strcmp(nickname[i],from)==0){
											for(int j=2;j<MAX_CLIENTS+2;j++){
												if(rfds[j].fd==client_id[i]){
													char error_message[] = "cannot find user\n";
													send(rfds[j].fd, error_message, sizeof(error_message),0);
													continue;
												}	
											}
										}
									}
								}

								// successfully sended 
								else{
									if(strcmp(from,to)==0) continue;
									snprintf(combined_message, sizeof(combined_message), "[%s]>>>[%s] : %s",from,to,message);
									for(int i=0;i<current_client;i++){
										if(strcmp(nickname[i],to)==0){
											for(int j=2;j<MAX_CLIENTS+2;j++){
												if(rfds[j].fd == client_id[i]){
													send(rfds[j].fd, combined_message, sizeof(combined_message),0);
													continue;
												}
											}
										}
									}
								}
							}

							// normal message
							else{
								printf("%s : %s",served_nickname, whisper);
								for(int i=2;i<MAX_CLIENTS+2;i++){
									if(rfds[i].fd<0) continue;
									snprintf(combined_message, sizeof(combined_message),"%s : %s",served_nickname, whisper);
									send(rfds[i].fd,combined_message,sizeof(combined_message),0);
								}
								
							}
                        }
                    }
                }
            }
            if(nKeepRunning==0) break;
            else if(nRetval<0) break;
        }
    }while(1);

        for(int i=0;i<MAX_CLIENTS+2;i++){
            if(rfds[i].fd>0) close(rfds[i].fd);
        }
        printf("Bye~\n");
        return 0;
    }

