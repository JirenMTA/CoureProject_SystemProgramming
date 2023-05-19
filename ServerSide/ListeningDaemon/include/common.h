#ifndef MYLIB_H
#define MYLIB_H
#include "type_packet.h"
#include <vector>
#include <string>

using namespace std;



extern const char* SOCKET_PATH;
extern const char* working_directory;
 
int read_setting_file();
void send_descript(int& incomming_fd, int& fd_to_send);
void create_socket(int& fd_to_create);
int get_uid_from_socket(int& incoming_fd);
void send_request(int& fd, request& pkg);
void add_new_user_in_storage(int uid);
bool check_exits_file_in_user(char* relative_path);
void send_back_result_analist(response& res, int& fd);
void receive_back_result_analist(response& res, int& fd);
right_t get_right_from_acl(int my_uid, int owner, const char* filename);
void update_right_acl(const std::vector<line_right>& list, const char* path_to_acl);
void insert_right_acl(int owner, const char* filename, int right, int receiver);
int send_storage(int& fd, std::vector<std::pair<string, bool>>& storage);
void list_dir_in_recursive(const char* current_dir);
std::vector<std::pair<string, bool>> list_dir(const char* current_dir);
response analist_requets(const request& pkg, int& fd);

#endif // MYLIB_H
