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
#include "Network_Common.h"
#include "db.h"

int main(int argc,char *argv[]){
    struct sockaddr_in stCAddr;
    socklen_t nCAddr;
    struct pollfd rfds[2];
    int nTimeout=0;
    int nRetval;
	struct tm *current_time;
	time_t t;
	char nickname[11];

	// Initialize file descripter
    for(int i=0;i<2;i++){
        rfds[i].fd=-1;
        rfds[i].events=0;
        rfds[i].revents=0;
    }

    if(argc!=3){
        printf("Usage : %s <IP> <Port>\n",argv[0]);
        return -1;
    }

    printf("IP %s\n",argv[1]);
    printf("Port %s\n",argv[2]);

	// stdin >> fd[0]
    rfds[0].fd=0;
    rfds[0].events=POLLIN;
    rfds[0].revents=0;

	// trying to connect to TCP server
    rfds[1].fd=connect_to_tcp_server(argv[1],atoi(argv[2]));

    if(rfds[1].fd<0){
        printf("Connecting Server is failed.\n");
        return -1;
    }


	// =============== start to connect db ==================
	start_db();
	// create nickname
	while(1){
		printf("Enter a string (up to 10 characters)>>>");
		fgets(nickname,sizeof(nickname),stdin);
		int flag = create_nickname(nickname);
		if(flag) continue;
		else{
			send(rfds[1].fd,nickname,sizeof(nickname),0);
			break;
		}
	}

	// =============== start chat ===================

    rfds[1].events=POLLIN;
    rfds[1].revents=0;
    nTimeout=1000;

    do{
		// poll...event monitering
        nRetval=poll(rfds,2,nTimeout);
        if(nRetval>0){
			// in occurs on FD[0]
            if(rfds[0].revents&POLLIN){
				// send data to server
                char message[BUFSIZ];
				char combined_message[BUFSIZ];
                int message_len=0;
				// time 
				t = time(NULL);
				current_time = localtime(&t);
				char timestamp[20];
				strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", current_time);

                bzero(message,BUFSIZ);
                message_len=read(0,message,BUFSIZ);
                if(message_len>0){
                    if(strncasecmp(message,"exit",4)==0){
						char disconnect_message[500];
						snprintf(disconnect_message,sizeof(disconnect_message),"%s has terminated the client.\n",nickname);
						send(rfds[1].fd,disconnect_message,sizeof(disconnect_message),0);
						printf("DISCONNECT\n");
						break;
					}
					else{
						snprintf(combined_message, sizeof(combined_message), "[%s]>>>%s", nickname, message);
                    	send(rfds[1].fd,combined_message,sizeof(combined_message),0);
					}
					// insert message log
//					const char *insert_message = "insert into message (message_text,send_time,nickname) values (?,?,?);";
//					sqlite3_stmt *stmt;
//					rc = sqlite3_prepare_v2(db,insert_message,-1,&stmt,0);
//					if(rc!=SQLITE_OK){
//						printf("insert SQL error : %s\n",sqlite3_errmsg(db));
//					}
//					sqlite3_bind_text(stmt,1,message,-1,SQLITE_STATIC);
//					sqlite3_bind_text(stmt,2,timestamp,-1,SQLITE_STATIC);
//					sqlite3_bind_text(stmt,3,nickname,-1,SQLITE_STATIC);
//					rc = sqlite3_step(stmt);
//					sqlite3_finalize(stmt);
//

					//snprintf(combined_message, sizeof(combined_message), "[%s]>>>%s", nickname, message);
                    //send(rfds[1].fd,combined_message,sizeof(combined_message),0);
                }
            }
       
         if(rfds[1].revents&POLLIN){
			 // read data from server
            char strBuffer[BUFSIZ];
            int nBufferLen=0;
            bzero(strBuffer,BUFSIZ);
            nBufferLen=read(rfds[1].fd,strBuffer,BUFSIZ);
            printf("%s",strBuffer);
        	}
        	else if(nRetval<0) break;
    	}
    }while(1);
    close(rfds[1].fd);
//	sqlite3_close(db);
    return 0;
}
