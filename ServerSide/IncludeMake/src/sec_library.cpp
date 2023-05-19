#include "sec_library.h"
#include "type_packet.h"
#include "common.h"
#include "functions_of_types.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <vector>
#include <sys/stat.h>
#include <iostream>
#include <string>
#include <dirent.h>
#include <nlohmann/json.hpp>
#include <libconfig.h>
#include <pwd.h>
#include <cstring>
using json = nlohmann::json;
using namespace std;


int fd_connect_to_daemon;

std::istream& operator>>(std::istream& is, right_t& r)
{
	int r_int;
	is >> r_int;
	if (r_int <=0)
	{
		r = R_NONE;
		return is;
	}
	if (r_int >= 7)
	{
		r = R_ALL;
		return is;
	}
	r = right_t(r_int);
	return is;
}

std::ostream& operator<<(std::ostream& os, const right_t& r)
{
	os << convert_right_to_string(r);
	return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<std::pair<string, bool>> list_storage)
{
	if (!list_storage.size())
	{
		os << "Current dir empty!" << endl;
		return os;
 	}
	int index_file = 0;
	for (auto file: list_storage)
	{
		if (file.second)
			os << "[DIR] " << file.first << endl;
		else
		{
			os << "[" << index_file << "] " << file.first << endl;
			index_file++;
		}
	}
	return os;
}


#pragma region INNITIALIZE
int sec_init()
{
	read_setting_file();
	printf("SOCKET PATH: %s\n", SOCKET_PATH);
	int res;
	fd_connect_to_daemon = socket(AF_UNIX, SOCK_SEQPACKET, 0);
    sockaddr_un socketAddr;
	memset(&socketAddr, 0, sizeof(sockaddr_un));
    socketAddr.sun_family = AF_UNIX;
    strncpy(socketAddr.sun_path, SOCKET_PATH, sizeof(socketAddr.sun_path) -1);
    res = check(connect(fd_connect_to_daemon, (sockaddr*) &socketAddr, sizeof(socketAddr)));
	if (res < 0)
		return -1;
	return fd_connect_to_daemon;
}
#pragma endregion



#pragma region REQUEST_OPEN_DESCRIPTOR
int receive_descript(int& fd_received)
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
	int r = recvmsg(fd_connect_to_daemon, &msg, MSG_EOR);
	auto cmsg = CMSG_FIRSTHDR(&msg);
	fd_received = *(int*)CMSG_DATA(cmsg);
	if (fd_received < 0)
		return -1;
	return fd_received; 
}

int sec_openat(int uid, const char* filename, mode_t mode)
{
	struct request pkg{};
	struct response res{};
	int fd_received = -1;
	strcpy(pkg.filename, filename);
	pkg.target_id = uid;
	pkg.req = TYPE::REQ_OPEN;
	pkg.mode = mode;
	
	send_request(fd_connect_to_daemon, pkg);
	receive_back_result_analist(res, fd_connect_to_daemon);
	receive_descript(fd_received);
	
	if (!res.result_code)
	{
		fd_received = -1;
		return fd_received;
	}
	return fd_received;
}

int sec_open(const char* filename, mode_t mode)
{
	return sec_openat(getuid(), filename, mode);
}
#pragma endregion

#pragma region REQUEST_CHECK_RIGHT

right_t sec_check(int uid, const char* filename, right_t rights)
{
	struct request pkg{};
	struct response res{};
	strcpy(pkg.filename, filename);
	pkg.right = right_t::R_NONE;
	pkg.req = TYPE::REQ_GET_RIGHTS;
	pkg.target_id = uid;
	send_request(fd_connect_to_daemon, pkg);
	receive_back_result_analist(res, fd_connect_to_daemon);
	return res.right;
}
#pragma endregion

#pragma region REQUEST_LIST_DIR
void out_put_list_dir(std::vector<std::pair<string, bool>> elems_dir)
{
	if (!elems_dir.size())
	{
		cout << "Current dir empty!" << endl;
		return;
 	}
	int index_file = 0;
	for (auto file: elems_dir)
	{
		if (file.second)
			cout << KBLU << "[DIR] " << file.first << KNRM << endl;
		else
		{
			cout << KGRN << "["<< index_file << "] " << file.first << KNRM << endl;
			index_file++;
		}
	}
}

int send_storage(int& fd, std::vector<std::pair<string, bool>>& storage)
{
	std::string json_str = json(storage).dump();

    // Send the JSON string size and data over the socket.
    const char *msg = json_str.c_str();
	size_t len = json_str.size();
	send(fd, msg, len, 0);
	return 0;
}

int receive_storage(int& fd, std::vector<std::pair<string, bool>>& storage)
{
	char* buf = new char[1024];
	int n = recv(fd, buf, 1024, 0);

	std::string json_str(buf, n);
	storage = nlohmann::json::parse(json_str);
	return 0;
}

std::vector<std::pair<string, bool>> sec_list_storage(int target_uid)
{
	struct request pkg{};
	struct response res{};
	std::vector<std::pair<string, bool>> storage;
	storage.clear();
	pkg.req = REQ_GET_STORAGE;
	pkg.target_id = target_uid;
	pkg.right = right_t::R_ALL;
	send_request(fd_connect_to_daemon, pkg);
	receive_back_result_analist(res, fd_connect_to_daemon);
	receive_storage(fd_connect_to_daemon, storage);
	return storage;
}

#pragma endregion

#pragma region REQUEST_GRANT
right_t sec_grant(int uid, const char* filename, right_t right)
{
	request message{};
	strcpy(message.filename,filename);
	message.target_id = uid;
	message.right = static_cast<right_t>(right);
	message.req = REQ_GRANT;
	send_request(fd_connect_to_daemon, message);
	struct response res{};
	receive_back_result_analist(res, fd_connect_to_daemon);
	return res.right;
}
#pragma endregion

#pragma region REQUEST_REWOKE
right_t sec_revoke(int uid, const char* filename, right_t right)
{
	request message{};
	response res{};
	strcpy(message.filename,filename);
	message.target_id = uid;
	message.right = right;
	message.req = REQ_REVOKE;
	message.mode = 0x777;
	send_request(fd_connect_to_daemon, message);
	receive_back_result_analist(res, fd_connect_to_daemon);
	return res.right;
}
#pragma endregion

#pragma region REQUEST_DELETE
int sec_unlink_at(int target_uid, const char* filename)
{
	struct  request pkg{};
	struct response res{};
	strcpy(pkg.filename, filename);
	pkg.target_id = target_uid;
	pkg.req = TYPE::REQ_UNLINK;
	send_request(fd_connect_to_daemon, pkg);
	receive_back_result_analist(res, fd_connect_to_daemon);
	if(res.result_code)
		return 0;
	return -1;
}
#pragma andregion
