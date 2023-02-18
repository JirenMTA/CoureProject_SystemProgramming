
# pragma once
#include "check.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <vector>
#include <sys/stat.h>
#define SOCKET_PATH "/tmp/resol.socket"
#include <iostream>

using namespace std;



bool receive_descript(int&fd_connected, int& fd_received)
{
	char buff[32];
	char data_buffer[1];
	iovec data_iov{.iov_base = data_buffer, .iov_len = 1};
	alignas(cmsghdr)
	char spec_buffer[CMSG_SPACE(sizeof(int))];
	memset(spec_buffer, '\0', sizeof(spec_buffer));
	msghdr msg{};
	msg.msg_iov = &data_iov;
	msg.msg_iovlen = 1;
	msg.msg_control = spec_buffer;
	msg.msg_controllen = sizeof(spec_buffer);
	
	int r = recvmsg(fd_connected, &msg, MSG_EOR);
	

	auto cmsg = CMSG_FIRSTHDR(&msg);
	fd_received = *(int*)CMSG_DATA(cmsg);


    return true;
}

void create_socket(int& listen_fd)
{
	umask(0);
	listen_fd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
	sockaddr_un addr{};
	addr.sun_family = AF_UNIX;
	char name[] = SOCKET_PATH;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) -1);
	unlink(name);

	bind(listen_fd, (sockaddr*)&addr, sizeof(addr));
	listen(listen_fd, 10);
}

void send_descript(int& fd_connected, int& fd_to_send)
{
	char data_buffer[1];
	iovec data_iov{.iov_base = data_buffer, .iov_len = 1};
	alignas(cmsghdr)
	char spec_buffer[CMSG_SPACE(sizeof(int))];
	memset(spec_buffer, '\0', sizeof(spec_buffer));
	msghdr msg{};
	msg.msg_iov = &data_iov;
	msg.msg_iovlen = 1;
	msg.msg_control = spec_buffer;
	msg.msg_controllen = sizeof(spec_buffer);
	
	if (fd_to_send < 0)
	{
		int fd_fake = 0;
		cmsghdr* chdr    = CMSG_FIRSTHDR(&msg);
		chdr->cmsg_len   = sizeof(spec_buffer);
		chdr->cmsg_level = SOL_SOCKET; //UNIX socket level
		chdr->cmsg_type  = SCM_RIGHTS; //transferring descriptors
		*(int*)CMSG_DATA(chdr) = fd_fake;
		check(sendmsg(fd_connected, &msg, 0));
	}
	else
	{
		cmsghdr* chdr    = CMSG_FIRSTHDR(&msg);
		chdr->cmsg_len   = sizeof(spec_buffer);
		chdr->cmsg_level = SOL_SOCKET; //UNIX socket level
		chdr->cmsg_type  = SCM_RIGHTS; //transferring descriptors

		*(int*)CMSG_DATA(chdr) = fd_to_send;
		check(sendmsg(fd_connected, &msg, 0));
	}
}

int get_uid_from_socket(int& incoming_fd)
{
	socklen_t len;
	struct ucred ucred;
	len = sizeof(struct ucred);
	if (getsockopt(incoming_fd, SOL_SOCKET, SO_PEERCRED, &ucred, &len) == -1)
		return -1;
	return ucred.uid;

}



