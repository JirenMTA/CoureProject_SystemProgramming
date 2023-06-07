#pragma once
#include <iostream>
#include <StorageLib/sec_library.h>
#include <cstring>
#include <algorithm>
#include <pwd.h>

using namespace std;

// TODO
// TODO clean buffer
uid_t get_uid(std::string user){
    auto it = std::find_if(user.begin(), user.end(), [](char const &c) {
        return !std::isdigit(c);
    });

    return it == user.end() ? std::stoi(user) : getpwnam(user.c_str())->pw_uid;
}

std::string input_passwd(){
    std::string passwd;
    std::cin >> passwd;

    return std::to_string(std::hash<std::string>{}(passwd));
}

bool authorization(int uid, std::string& filename){
    if (user_in_ban_list(uid, filename.c_str())) {
        std::cout << "User in ban list " << std::endl;
        return false;
    }
    if(passwd_exists(uid, filename.c_str())) {
        std::cout << "Input passwd ";
        std::string passwd = input_passwd();
        if (!authorization_by_passwd(uid, filename.c_str(), passwd.c_str())) {
            std::cout << "Authorization fail ";
            return false;
        }
    }
    return true;
}

void task_try_read()
{
    std::string filename;
    std::string owner;
    char buff[64];
    memset(buff, 0, sizeof(buff));
    std::cout << ">> Input file name: ";
    std::cin >> filename;
    std::cout << ">> Input owner: ";
    std::cin >> owner;
    int uid = get_uid(owner);

    if(!authorization(uid, filename))
        return;

    int fd_received = sec_openat(uid, filename.c_str(), 0777);
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
    std::string filename;
    std::string owner;
    char buff[64];
    memset(buff, 0, sizeof(buff));
    std::cout << ">> Input file name: ";
    std::cin >> filename;
    std::cout << ">> Input owner: ";
    std::cin >> owner;
    int uid = get_uid(owner);

    if(!authorization(uid, filename))
        return;

    string str_buff;
	cin.get();

	int fd_received = sec_openat(uid, filename.c_str(), 0777);
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

    std::cout << ">> Input file name: ";
    std::cin >> filename;
    std::cout << ">> Input owner: ";
    std::cin >> owner;
    int uid = get_uid(owner);

    if(!authorization(uid, filename))
        return;

    right_t r;

	r = sec_check(uid, filename.c_str(), R_NONE);
	cout << "You have right: " << r << endl;

}
void task_try_grant()
{
    std::string filename;
    std::string owner;

    right_t r;

    std::cout << ">> Input file name: ";
    std::cin >> filename;
    std::cout << ">> Input owner: ";
    std::cin >> owner;
    int uid = get_uid(owner);

    if(!authorization(uid, filename))
        return;

    cout << ">> Input right: ";
    cin >> r;
    r = sec_grant(uid, filename.c_str(), r);
    cout << "Result right: " << r << endl;
}
void task_try_list_storage()
{
    std::string owner;
	std::vector<std::pair<string, bool>> res_list;
	std::cout << ">> Input owner: ";
	std::cin >> owner;

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

    int uid = get_uid(owner);
    if(!authorization(uid, filename))
        return;

	r = sec_revoke(uid, filename.c_str(), r);
}
void task_try_delete()
{
    std::string filename;
    std::string owner;

    std::cout << ">> Input file name: ";
    std::cin >> filename;
    std::cout << ">> Input owner: ";
    std::cin >> owner;

    int uid = get_uid(owner);
    if(!authorization(uid, filename))
        return;

    int res_delete = sec_unlink_at(uid, filename.c_str());

    cout << "Result delete: " << res_delete << endl;
}

#pragma region daria


void task_try_ban_user() {
    std::string filename;
    std::string owner;

    std::cout << ">> Input filename: ";
    std::cin >> filename;
    std::cout << ">> Input ban user: ";
    std::cin >> owner;

    int uid = get_uid(owner);

    if(!authorization(uid, filename))
        return;

    int res = sec_ban_user(uid, filename.c_str());

    cout << "Result: " << res << endl;
}
void task_try_unban_user() {
    std::string filename;
    std::string owner;

    std::cout << ">> Input filename: ";
    std::cin >> filename;
    std::cout << ">> Input unban user: ";
    std::cin >> owner;

    int uid = get_uid(owner);

    if(!authorization(uid, filename))
        return;

    int res = sec_unban_user(uid, filename.c_str());

    cout << "Result: " << res << endl;
}
void task_passwd_by_file() {
    std::string filename;
    std::string owner;
    std::string passwd;

    std::cout << "Input owner: ";
    std::cin >> owner;
    std::cout << "Input file: ";
    std::cin >> filename;
    int uid = get_uid(owner);
    if(!authorization(uid, filename))
        return;

    std::cout << "Input passwd ";
    passwd = input_passwd();

    int res = sec_passwd_by_file(uid, filename.c_str(), passwd.c_str());
    std::cout << res;
}


#pragma endregion daria









