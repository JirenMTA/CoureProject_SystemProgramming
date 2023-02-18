#include <iostream>
#include <sys/stat.h>
#include "../Include/common.h"

using namespace std;


int main()
{
	char msg[32];
	int socketFd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    sockaddr_un socketAddr;
	memset(&socketAddr, 0, sizeof(sockaddr_un));
    socketAddr.sun_family = AF_UNIX;
    strncpy(socketAddr.sun_path, SOCKET_PATH, sizeof(socketAddr.sun_path) -1);
    check(connect(socketFd, (sockaddr*) &socketAddr, sizeof(socketAddr)));
	while (true)
	{
		cout << "Input message: ";
		cin >> msg;
		check(send(socketFd, msg, 8, MSG_WAITALL));
	}
	return 0;

}
 