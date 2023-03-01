#pragma once
#include "common.cpp"
#include "type_packet.h"

int read_setting_file();
bool receive_descript(int& fd_received);
void create_socket(int& fd_to_create);
void send_descript(int& incomming_fd, int& fd_to_send);
int get_uid_from_socket(int& incoming_fd);
void send_request(int& fd, package_message& pkg);
int sec_init();
void add_new_user_in_storage(int uid);
bool check_exits_file_in_user(char* relative_path);
void send_back_result_analist(res_analist& res, int& fd);
void receive_back_result_analist(res_analist& res, int& fd);
right_t get_right_from_acl(int my_uid, int owner, const char* filename);
res_analist analist_requets(const package_message& pkg, int& fd);
int sec_open(const char* filename, mode_t mode);
