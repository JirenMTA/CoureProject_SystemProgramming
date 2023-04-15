#ifndef MYLIB_H
#define MYLIB_H
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
#include "type_packet.h"
#include <vector>
#include <string>
using json = nlohmann::json;

using namespace std;
extern const char* SOCKET_PATH;
extern const char* working_directory;
 

int read_setting_file();
void send_request(int& fd, request& pkg);
void receive_back_result_analist(response& res, int& fd);

#endif // MYLIB_H
