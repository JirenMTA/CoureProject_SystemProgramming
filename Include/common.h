#pragma once
#include "common.cpp"
#include "type_packet.h"

int read_setting_file();
void create_socket(int& fd_to_create);

int get_uid_from_socket(int& incoming_fd);
void send_request(int& fd, package_message& pkg);
int sec_init();
void add_new_user_in_storage(int uid);
bool check_exits_file_in_user(char* relative_path);
void send_back_result_analist(res_analist& res, int& fd);
void receive_back_result_analist(res_analist& res, int& fd);
right_t get_right_from_acl(int my_uid, int owner, const char* filename);
res_analist analist_requets(const package_message& pkg, int& fd);


#pragma region REQUEST_OPEN_DESCRIPTOR
void receive_descript(int& fd_received);
void send_descript(int& incomming_fd, int& fd_to_send);
int sec_openat(int uid, const char* filename, mode_t mode);
int sec_open(const char* filename, mode_t mode);
#pragma endregion

#pragma region REQUEST_CHECK_RIGHT
right_t sec_check(int uid, const char* filename, right_t rights);
#pragma endregion

#pragma region REQUEST_LIST_DIR
void list_dir_in_recursive(const char* current_dir);
std::vector<std::pair<string, bool>> list_dir(const char* current_dir);
void out_put_list_dir(std::vector<std::pair<string, bool>> elems_dir);
std::vector<std::pair<string, bool>> sec_list_storage(int target_uid);
int send_storage(int& fd,  std::vector<std::pair<string, bool>>& storage);
int receive_storage(int& fd, std::vector<std::pair<string, bool>>& storage);
#pragma endregion