#include "check.hpp"
#include "../include/type_packet.h"
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

#include <fstream>


using json = nlohmann::json;
using namespace std;


const char* SOCKET_PATH;
const char* working_directory;
 
int read_setting_file()
{
	config_t cfg;
	config_init(&cfg);
    
    if (!config_read_file(&cfg, "/usr/local/bin/config.cfg")) {
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
	setuid(getuid());
	struct passwd* pw = getpwuid(getuid());
	umask(0);
	fd_to_create = socket(AF_UNIX, SOCK_SEQPACKET, 0);
	sockaddr_un addr{};
	addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path));
	unlink(SOCKET_PATH);
	if (strcmp("SEC_OPERATOR", pw->pw_name) && strcmp("root", pw->pw_name))
	{
		printf("No permission to run daemon\n");
		exit(-1);
	}

	bind(fd_to_create, (sockaddr*)&addr, sizeof(addr));
	listen(fd_to_create, 10);
	chown(SOCKET_PATH, pw->pw_uid, pw->pw_gid);
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
int get_uid_from_socket(int& incoming_fd)
{
	socklen_t len;
	struct ucred ucred;
	len = sizeof(struct ucred);
	if (getsockopt(incoming_fd, SOL_SOCKET, SO_PEERCRED, &ucred, &len) < 0)
		return -1;
	return ucred.uid;

}

void send_request(int& fd, request& pkg)
{
	check(send(fd, (void*)&pkg, sizeof(pkg), MSG_WAITALL));
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
    return stat(relative_path, &sb) == 0 ? true : false;
}

void send_back_result_analist(response& res, int& fd)
{
	check(send(fd, (const void*)&res, sizeof(response), MSG_WAITALL));
}

void receive_back_result_analist(response& res, int& fd)
{
	check(recv(fd, (void*)&res, sizeof(response), MSG_WAITALL));
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
			return static_cast<right_t>(line.right);
		}
	}
	fclose(file);

	return right_t::R_NONE;
}

void update_right_acl(const std::vector<line_right>& list, const char* path_to_acl)
{
	FILE* file = fopen(path_to_acl, "w+");
	if (!file) {
		cout << "Error open file: " << path_to_acl << endl;
		return;
	}
	for (auto right_in_list: list)
	{
		fprintf(file, "%d %s %d\n", right_in_list.uid, right_in_list.filename,
                    (int)(right_in_list.right));
	}
	fclose(file);
}

void insert_right_acl(int owner, const char* filename, int right, int receiver)
{
	char path_to_file[64];
	int fd_acl;
	struct stat st;
	memset(path_to_file, 0, sizeof(path_to_file));
	sprintf(path_to_file, "./acl/%d", owner);
	struct line_right line;
	std::vector<line_right> list_line;

	stat(path_to_file, &st);
	if (st.st_size > 0)   // check for acl-file non-empty
	{
		FILE* file = fopen(path_to_file, "r");
		if (!file) {
			cout << "Error open file: " << path_to_file << endl;
			return;
		}
		while (!feof(file))
		{
			fscanf(file, "%d %s %d\n", &line.uid, line.filename, &line.right);
			list_line.push_back(line);
		}
		fclose(file);
	}

	line_right ln;
	ln.uid = receiver;
	ln.right = right;
	strcpy(ln.filename, filename); 

	for (int i =0; i< list_line.size(); i++)
	{
		if (list_line[i].uid == ln.uid && !strcmp(list_line[i].filename, ln.filename))
		{
			list_line[i].right = ln.right;
			update_right_acl(list_line, path_to_file);
			return;
		}
	}
	
	list_line.push_back(ln);
	update_right_acl(list_line, path_to_file);
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
            result_static_list_dir.push_back(std::pair<string, bool>(std::string(current_dir) +
                                        std::string("/") + std::string(entry->d_name), true));

            list_dir_in_recursive(path);
        } 
		else {

            std::string current_file = std::string(current_dir) + std::string("/")
                                    + std::string(entry->d_name);
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

void ban_user_by_uid(int uid, const char* filename) {
    const char path_to_ban_users[] = "/home/SEC_OPERATOR/ban_user.txt";

    char name[64];
    sprintf(name, "%d %s", uid, filename);

    ofstream fileOUT(path_to_ban_users, ios::app); // open filename.txt in append mode
    fileOUT << name << endl;
    fileOUT.close();
}

void set_passwd(const int uid, const char* filename, const char* passwd){

    char path_to_info_file[64];
    sprintf(path_to_info_file, "/home/SEC_OPERATOR/%d/__info.txt", uid);

    char data[128];
    sprintf(data, "%s %s", filename, passwd);

    ofstream fileOUT(path_to_info_file, ios::app);
    fileOUT << data << endl;
    fileOUT.close();

}


response analist_requets(const request& pkg, int& fd)
{
	int fd_test_exist;
	struct response res{};
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
		case REQ_GRANT:
			res.right = pkg.right;
			sprintf(res.path_to_file, "./%d/%s", res.uid, pkg.filename);
			fd_test_exist = open(res.path_to_file, O_RDONLY, 0x777);
			if (fd_test_exist >= 0)
			{
				close(fd_test_exist);
				res.right = pkg.right;
				insert_right_acl(res.uid, pkg.filename, pkg.right, res.target_uid);
				res.result_code = true;
			}
			else
			{
				res.right = R_NONE;
				res.result_code = false;
			}
			break;
		case REQ_REVOKE:
			res.right = pkg.right;
			sprintf(res.path_to_file, "./%d/%s", res.uid, pkg.filename);
			fd_test_exist = open(res.path_to_file, O_RDONLY, 0x777);
			if (fd_test_exist >= 0)
			{
				close(fd_test_exist);
				current_r = get_right_from_acl(res.target_uid, res.uid, pkg.filename);
				cout << current_r << endl;
				res.right = (right_t)(current_r & (~pkg.right));
				insert_right_acl(res.uid, pkg.filename, res.right, res.target_uid);
				res.result_code = true;
			}
			else
			{
				res.right = R_NONE;
				res.result_code = false;
			}
			break;
		case REQ_UNLINK:
			int res_del;
			sprintf(res.path_to_file, "./%d/%s", res.target_uid, pkg.filename);
			if (res.target_uid == res.uid)
				res.right = right_t::R_ALL;
			else
				res.right = get_right_from_acl(res.uid, res.target_uid, pkg.filename);
			if (can_delete(res.right))
			{
				res_del = unlink(res.path_to_file);
				if (res_del < 0)
					res.result_code = false;
				else
					res.result_code = true;
			}
			else
				res.result_code = false;
			break;

        case REQ_BAN_USER:
            int res_ban;
            sprintf(res.path_to_file, "./%d/%s", res.uid, pkg.filename);
            ban_user_by_uid(res.target_uid, res.path_to_file);
            res.result_code = true;
            break;

        case REQ_SET_PASSWD:
            sprintf(res.path_to_file, "./%d/%s", res.uid, pkg.filename);
            set_passwd(res.uid, pkg.filename, pkg.passwd);
            res.result_code = true;
            break;
	}
	return res;
}
