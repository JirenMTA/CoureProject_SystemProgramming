#include <iostream>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include "../Include/common.h"
using namespace std;


int main()
{
	char data[32];
	sec_init();
	int fd;
	
	fd = sec_openat(1002, "test1", 0x777);
	read(fd, data, sizeof(data));
	cout << "Got data of file: " << data << endl;

	return 0;

}


 