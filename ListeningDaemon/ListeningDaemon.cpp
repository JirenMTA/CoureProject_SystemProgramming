#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <syslog.h>
#include <cstring>
#include <signal.h>
#include <pwd.h>
#include "../Include/common.h"
#include "../Include/type_function.cpp"
using namespace std;


#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"


/*

sudo useradd -m -G adm,cdrom,sudo -s /bin/bash SEC_OPERATOR
id -u <username>
tail -n 10 /var/log/syslog      // to read log-file (message from daemon) (10 last row in file)
grep <whatfind> /var/log/syslog // to read log-file (message from daemon)
ps -jx | grep <program's name>    // to list all running daemon in terminal
cp  sec_storage.service /etc/systemd/system
cp <Program> /usr/local/bin/
cpp.sh <cpp shell>
list user: /etc/shadow
*/


static void skeleton_daemon()
{
	pid_t pid;
	pid = fork();
	if (pid < 0)
		exit(EXIT_FAILURE);
	if (pid > 0)
		exit(EXIT_SUCCESS);
	if (setsid() < 0)
		exit(EXIT_FAILURE);
	signal(SIGCHLD, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	pid = fork();
	if (pid < 0)
		exit(EXIT_FAILURE);
	if (pid > 0)
		exit(EXIT_SUCCESS);
	umask(0);
	chdir("/");
	int x;
	for (x = sysconf(_SC_OPEN_MAX); x>=0; x--)
	{
		close (x);
	}
	openlog ("Server_Daemon", LOG_PID, LOG_DAEMON);
}

void accept_client(int& listen_fd)
{
	char buff[32];
	int flag = 1;
	int res, fd_to_send, incoming_fd;
	syslog(LOG_NOTICE, "Started listening");
	while (true)
	{	
		socklen_t incoming_len;
		sockaddr_un incoming_address;
		string str_right;
		int result_delete;
		char msg[32];
		incoming_fd = accept(listen_fd, (sockaddr*)&incoming_address, &incoming_len);
		int id;
		pid_t p = fork();
		if (!p)
		{
			id = get_uid_from_socket(incoming_fd);
			cout << KMAG <<"Client " << id << " connected!" << KNRM << endl;
			add_new_user_in_storage(id);
			while (true)
			{
				package_message msg;
				res = check(recv(incoming_fd, (void*)&msg, sizeof(msg), MSG_WAITALL));
				check(res);
				if (errno == ENOTCONN || res < sizeof(package_message))
					break;
				struct res_analist analist_req = analist_requets(msg, incoming_fd);

				if (analist_req.req == REQ_OPEN)
				{
					cout << "\n>> Client " << analist_req.uid << " send request to file " << analist_req.path_to_file << " of: " << analist_req.target_uid << endl;  
					cout << ">> Got right: " << convert_right_to_string(analist_req.right) << endl;
					if (analist_req.right >= 0)
					{	
						flag = get_flag_open_from_right(analist_req.right);
						fd_to_send = open(analist_req.path_to_file, flag, analist_req.mode);

						cout << "fd = " << fd_to_send << endl;
						if (fd_to_send < 0)
							analist_req.result_code = false;
						else
							analist_req.result_code = true;
						send_back_result_analist(analist_req, incoming_fd);
						send_descript(incoming_fd, fd_to_send);
					}
				}	
			}
			cout << "Client " << id << " disconnected!" << endl;
		}
	}
}

//
//Add new user 'SEC_OPERATOR' before run server
// command: sudo useradd -m -G adm,cdrom,sudo -s /bin/bash SEC_OPERATOR


int main()
{

	char cwd[64];
	struct passwd* pass;
	pass = getpwuid(getuid());
	if (strcmp("SEC_OPERATOR", pass->pw_name))
	{
		printf("User must be SEC_OPERTOR!\n");
		exit(-1);
	}
	read_setting_file();

	remove(SOCKET_PATH);
	chdir(working_directory);

	struct sigaction act{};
	act.sa_flags = SA_NOCLDWAIT;
	sigaction(SIGCHLD, &act, NULL);

	getcwd(cwd, sizeof(cwd));
	cout << cwd << endl;
	int listen_fd;
	create_socket(listen_fd);
	accept_client(listen_fd);


	return EXIT_SUCCESS;
}


