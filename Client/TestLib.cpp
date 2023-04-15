#pragma once
#include <iostream>
#include <StorageLib/sec_library.h>


using namespace std;
void task_try_read()
{
	int target_uid;
	char buff[64];
	char filename[32];
	cout << ">> Input file name: ";
	cin >> filename;
	cout << ">> Input owner: ";
	cin >> target_uid;
	int fd_received = sec_openat(target_uid, filename, 0777);
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
	{
		cout << "Error when open file!" << endl;
	}
}
void task_try_write()
{
	char buff[64];
	int  owner;
	string str_buff;
	char filename[32];
	cout << ">> Input file name: ";
	cin >> filename;
	cout << ">> Input owner: ";
	cin >> owner;
	cin.get();
	int fd_received = sec_openat(owner, filename, 0777);
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
	char filename[32];
	int uid_owner;
	right_t r;
	cout << ">> Input file name: ";
	cin >> filename;
	cout << ">> Input owner: ";
	cin >> uid_owner;
	r = sec_check(uid_owner, filename, R_NONE);
	cout << "You have right: " << r << endl;
}

void task_try_grant()
{
	char filename[32];
	right_t r;
	int recv;

	cout << ">> Input filename: ";
	cin >> filename;

	cout << ">> Input receiver: ";
	cin >> recv;

	cout << ">> Input right: ";
	cin >> r;
	r = sec_grant(recv, filename, r);
	cout << "Result right: " << r << endl;
}

void task_try_list_storage()
{
	int owner;
	std::vector<std::pair<string, bool>> res_list;
	cout << ">> Input owner: ";
	cin >> owner;
	res_list = sec_list_storage(owner);
	cout << res_list;
}

void task_try_revoke()
{
	char filename[32];
	int recv;
	right_t r;
	cout << ">> Input file name: ";
	cin >> filename;
	cout << ">> Input receiver: ";
	cin >> recv;
	cout << ">> Input right-revoke: ";
	cin >> r;
	r = sec_revoke(recv, filename, r);
}

void task_try_delete()
{
	char filename[32];
	int uid;
	right_t r;
	cout << ">> Input file name: ";
	cin >> filename;
	cout << ">> Input owner: ";
	cin >> uid;
	int res_delete = sec_unlink_at(uid, filename);
	cout << "Result delete: " << res_delete << endl;
}











