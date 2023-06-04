#pragma once
#include <iostream>
#include <StorageLib/sec_library.h>
#include <cstring>
#include <algorithm>
#include <pwd.h>

using namespace std;

uid_t get_uid(std::string user){
    auto it = std::find_if(user.begin(), user.end(), [](char const &c) {
        return !std::isdigit(c);
    });

    return it == user.end() ? std::stoi(user) : getpwnam(user.c_str())->pw_uid;
}

void task_try_read()
{
    std::string filename;
    std::string owner;
    char buff[64];

    std::cout << ">> Input file name: ";
    std::cin >> filename;
    std::cout << ">> Input owner: ";
    std::cin >> owner;

    int fd_received = sec_openat(get_uid(owner), filename.c_str(), 0777);
    if (fd_received > 0)
    {
        if (read(fd_received, buff, 64) >= 0)
        {
            cout << "Got data: " << buff << endl;
            close(fd_received);
        }
        else
            cout << "No permission to read!" << endl;
    }
    else
        cout << "Error when open file!" << endl;
}
void task_try_write()
{
    char buff[64];
    std::string filename;
    std::string owner;
    string str_buff;

    std::cout << ">> Input file name: ";
    std::cin >> filename;
    std::cout << ">> Input owner: ";
    std::cin >> owner;

	cin.get();
	int fd_received = sec_openat(get_uid(owner), filename.c_str(), 0777);
	if (fd_received > 0)
	{
		cout << "Enter new data: ";
		getline(cin, str_buff);
		strcpy(buff, str_buff.c_str());
		write(fd_received, buff, sizeof(buff));
		close(fd_received);
	}
	else
	{
		cout << "No permission" << endl;
	}
}
void task_get_right()
{
    std::string filename;
    std::string owner;
    right_t r;

    std::cout << ">> Input file name: ";
    std::cin >> filename;
    std::cout << ">> Input owner: ";
    std::cin >> owner;

	r = sec_check(get_uid(owner), filename.c_str(), R_NONE);
	cout << "You have right: " << r << endl;
}

void task_try_grant()
{
    std::string filename;
    std::string owner;
    right_t r;

    std::cout << ">> Input filename: ";
    std::cin >> filename;
    std::cout << ">> Input receiver: ";
    std::cin >> owner;
    std::cout << ">> Input right: ";
    std::cin >> r;

	r = sec_grant(get_uid(owner), filename.c_str(), r);
	cout << "Result right: " << r << endl;
}
void task_try_list_storage()
{
    std::string owner;
	std::vector<std::pair<string, bool>> res_list;
	cout << ">> Input owner: ";
	cin >> owner;
	res_list = sec_list_storage(get_uid(owner));
	cout << res_list;
}

void task_try_revoke()
{
    std::string filename;
    std::string owner;
    right_t r;

    std::cout << ">> Input filename: ";
    std::cin >> filename;

    std::cout << ">> Input receiver: ";
    std::cin >> owner;

    std::cout << ">> Input right-revoke: ";
    std::cin >> r;

	r = sec_revoke(get_uid(owner), filename.c_str(), r);
}

void task_try_delete()
{
    std::string filename;
    std::string owner;

    std::cout << ">> Input file name: ";
    std::cin >> filename;
    std::cout << ">> Input owner: ";
    std::cin >> owner;

    int res_delete = sec_unlink_at(get_uid(owner), filename.c_str());

    cout << "Result delete: " << res_delete << endl;
}











