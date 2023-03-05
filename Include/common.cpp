
# pragma once
#include "check.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <vector>
#include <sys/stat.h>
#include <iostream>
#include <dirent.h>
#include "type_packet.h"
#include "type_function.cpp"
#include "common.h"
#include <nlohmann/json.hpp>
#include <libconfig.h>
using json = nlohmann::json;
using namespace std;

const char* SOCKET_PATH;
const char* working_directory;

static int fd_connect_to_daemon;

int read_setting_file()
{
	config_t cfg;
	config_init(&cfg);
    
    if (!config_read_file(&cfg, "../../Include/config.cfg")) {
		fprintf(stderr, "%s:%d - %s\n",
		config_error_file(&cfg),
		config_error_line(&cfg),
		config_error_text(&cfg));
		config_destroy(&cfg);
    return EXIT_FAILURE;
  	}
	if (config_lookup_string(&cfg, "SOCKET_PATH", &SOCKET_PATH) < 0) {
		printf("Error format setting file!\n");
		exit(-1);
	}
	if (config_lookup_string(&cfg, "working_directory", &working_directory)<0) {
		printf("Error format setting file!\n");
		exit(-1);
	}
	return 0;
}


void create_socket(int& fd_to_create)
{
	umask(0);
	fd_to_create = socket(AF_UNIX, SOCK_SEQPACKET, 0);
	sockaddr_un addr{};
	addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) -1);
	unlink(SOCKET_PATH);
	bind(fd_to_create, (sockaddr*)&addr, sizeof(addr));
	listen(fd_to_create, 10);
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

void send_request(int& fd, package_message& pkg)
{
	check(send(fd, (void*)&pkg, sizeof(pkg), MSG_WAITALL));
}

int sec_init()
{
	read_setting_file();
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

void add_new_user_in_storage(int uid)
{
	char user_dir[32], user_acl[32];
	sprintf(user_dir, "./%d", uid);
	sprintf(user_acl, "./acl/%d", uid);
	mkdir(user_dir, 0777);
	mkdir("./acl", 0777);
	open(user_acl, O_CREAT, 0777);
}

bool check_exits_file_in_user(char* relative_path)
{
	struct stat sb;
    if (stat(relative_path, &sb) == 0) {
        return true;
    } else {
		return false;
    }
	return true;
}

void send_back_result_analist(res_analist& res, int& fd)
{
	check(send(fd, (const void*)&res, sizeof(res_analist), MSG_WAITALL));
}

void receive_back_result_analist(res_analist& res, int& fd)
{
	check(recv(fd, (void*)&res, sizeof(res_analist), MSG_WAITALL));
}


right_t get_right_from_acl(int my_uid, int owner, const char* filename)
{
	char path_to_file[32];
	sprintf(path_to_file, "./acl/%d", owner);
	line_right line; 
	FILE* file = fopen(path_to_file, "r");
	if (!file) {
		cout << "Error open file: " << path_to_file << endl;
		return right_t::R_NONE;
	}
	while (!feof(file))
	{
		fscanf(file, "%d %s %d\n", &line.uid, line.filename, &line.right);
		if (my_uid == line.uid && !strcmp(filename, line.filename))
		{
			fclose(file);
			return (right_t)line.right;
		}
	}
	fclose(file);
	return right_t::R_NONE;
}

res_analist analist_requets(const package_message& pkg, int& fd)
{
	int fd_test_exist;
	struct res_analist res{};
	right_t current_r;
	res.req = pkg.req;
	res.uid = get_uid_from_socket(fd);
	res.target_uid = pkg.target_id;
	res.mode = pkg.mode;
	switch (pkg.req)
	{
		case REQ_OPEN:
			sprintf(res.path_to_file, "./%d/%s", res.target_uid, pkg.filename);
			if (res.target_uid == res.uid)
			{
				res.right = right_t::R_ALL;
				res.result_code = true;
				return res;
			}
			res.right = get_right_from_acl(res.uid, res.target_uid, pkg.filename);
			break;
		case REQ_GET_RIGHTS:
			sprintf(res.path_to_file, "./%d/%s", res.target_uid, pkg.filename);
			if (res.target_uid == res.uid)
				res.right = right_t::R_ALL;
			else
				res.right = get_right_from_acl(res.uid, res.target_uid, pkg.filename);
			break;
		case REQ_GET_STORAGE:
			int exits_storage;
			sprintf(res.path_to_file, "./%d", res.target_uid);
			exits_storage = open(res.path_to_file, O_RDONLY, 0x777);
			if (exits_storage < 0)
				res.result_code = false;
			else
				res.result_code = true;
			break;
	}
	return res;
}

#pragma region REQUEST_OPEN_DESCRIPTOR

void receive_descript(int& fd_received)
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
}

void send_descript(int& incomming_fd, int& fd_to_send)
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
		check(sendmsg(incomming_fd, &msg, 0));
	}
	else
	{
		cmsghdr* chdr    = CMSG_FIRSTHDR(&msg);
		chdr->cmsg_len   = sizeof(spec_buffer);
		chdr->cmsg_level = SOL_SOCKET; //UNIX socket level
		chdr->cmsg_type  = SCM_RIGHTS; //transferring descriptors

		*(int*)CMSG_DATA(chdr) = fd_to_send;
		check(sendmsg(incomming_fd, &msg, 0));
	}
}

int sec_openat(int uid, const char* filename, mode_t mode)
{
	struct package_message pkg{};
	struct res_analist res{};
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
	struct package_message pkg{};
	struct res_analist res{};
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

static std::vector<std::pair<string, bool>> result_static_list_dir;
void list_dir_in_recursive(const char* current_dir)
{
    DIR *dir;
    struct dirent *entry;
    if (!(dir = opendir(current_dir)))
        return;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            char path[1024];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            snprintf(path, sizeof(path), "%s/%s", current_dir, entry->d_name);
            result_static_list_dir.push_back(std::pair<string, bool>(std::string(current_dir) + std::string("/") + std::string(entry->d_name), true));

            list_dir_in_recursive(path);
        } 
		else {

            std::string current_file = std::string(current_dir) + std::string("/") + std::string(entry->d_name);
            result_static_list_dir.push_back(std::pair<string, bool>(current_file, false));
        }
    } 
}

std::vector<std::pair<string, bool>> list_dir(const char* current_dir)
{
	result_static_list_dir.clear();
	list_dir_in_recursive(current_dir);
	return result_static_list_dir;
}

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
	struct package_message pkg{};
	struct res_analist res{};
	std::vector<std::pair<string, bool>> storage;
	storage.clear();
	pkg.req = REQ_GET_STORAGE;
	pkg.target_id = target_uid;
	pkg.right = R_ALL;
	send_request(fd_connect_to_daemon, pkg);
	receive_back_result_analist(res, fd_connect_to_daemon);
	if (!res.result_code)
		return storage;
	receive_storage(fd_connect_to_daemon, storage);
	return storage;
}

#pragma endregion