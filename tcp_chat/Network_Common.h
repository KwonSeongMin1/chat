#include <sys/types.h>
#include <unistd.h>

int start_tcp_server(uint16_t nPort, int nBacklog);
int connect_to_tcp_server(char* strIpAddress, uint16_t nPort);

