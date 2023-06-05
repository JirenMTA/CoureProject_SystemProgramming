#ifndef SEC_LIBRARY
#define SEC_LIBRARY
#include <iostream>
#include <vector>
#include <check.hpp>
#include <type_packet.h>

using namespace std;

extern int fd_connect_to_daemon;

std::istream& operator>>(std::istream& is, right_t& r);
std::ostream& operator<<(std::ostream& os, const right_t& r);
std::ostream& operator<<(std::ostream& os, const std::vector<std::pair<string, bool>> list_storage);

#pragma region INNITALIZE
int sec_init();
#pragma endregion

#pragma region REQUEST_OPEN_DESCRIPTOR
int sec_openat(int uid, const char* filename, mode_t mode);
int sec_open(const char* filename, mode_t mode);
#pragma endregion

#pragma region REQUEST_CHECK_RIGHT
right_t sec_check(int uid, const char* filename, right_t rights);
#pragma endregion

#pragma region REQUEST_LIST_DIR
std::vector<std::pair<string, bool>> sec_list_storage(int target_uid);
#pragma endregion

#pragma region REQUEST_GRANT
right_t sec_grant(int uid, const char* filename, right_t right);
#pragma endregion

#pragma region REQUEST_REWOKE
right_t sec_revoke(int uid, const char* filename, right_t right);
#pragma endregion

// daria

#pragma region REQUEST_PASSWD_BY_FILE
bool passwd_exists(int uid, const char* filename);
bool authorization_by_passwd(int uid, const char* filename, const char* passwd);
int sec_passwd_by_file(int uid, const char* filename, const char* passwd);
#pragma endregion

#pragma region REQUEST_BAN_USER
int sec_ban_user(int uid, const char* filename);
int sec_unban_user(int uid, const char* filename);
#pragma endregion

// end daria

#pragma region REQUEST_DELETE
int sec_unlink_at(int target_uid, const char* filename);
#pragma andregion

#endif // SEC_LIBRARY
