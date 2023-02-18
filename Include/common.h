#pragma once
#include "common.cpp"
#include "check.hpp"
bool receive_descript(int&fd_connected, int& fd_received);
void create_socket(int& listen_fd);
void send_descript(int& fd_connected, int& fd_to_send);
int get_uid_from_socket(int& incoming_fd);
