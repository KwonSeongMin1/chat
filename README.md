# tcp 소켓을 활용한 채팅프로그램 with C
### 작업 환경 : Ubuntu 22.04.3 LTS
### 사용 언어 : C
### 사용 db : sqlite3


## 컴파일 하는 방법
```
gcc chat_client.c Network_Common.c db.c -o <filename> -lsqlite3
gcc chat_server.c Network_Common.c -o <filename>

sudo apt-get install libsqlite3-dev sqlite3 -y 필요
```
