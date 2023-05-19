#ifndef MYLIB_H
#define MYLIB_H
#include "type_packet.h"
using namespace std;
extern const char* SOCKET_PATH;
extern const char* working_directory;
 

int read_setting_file();
void send_request(int& fd, request& pkg);
void receive_back_result_analist(response& res, int& fd);

#endif // MYLIB_H
