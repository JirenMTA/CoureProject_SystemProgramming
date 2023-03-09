#include <sys/socket.h>
#include <syslog.h>
#include <signal.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <MyLib/common.h>
#include <MyLib/functions_of_types.h>
#include <MyLib/check.hpp>
using namespace std;

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
std::vector <int> connected_users;


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
	connected_users.clear();
	syslog(LOG_NOTICE, "Started listening");
	while (true)
	{	
		socklen_t incoming_len;
		sockaddr_un incoming_address;
		string str_right;
		int result_delete;
		char msg[32];
		//int id_connected;
		//get_uid_from_socket(incoming_fd);
		incoming_fd = accept(listen_fd, (sockaddr*)&incoming_address, &incoming_len);
		//id_connected = get_uid_from_socket(incoming_fd);
		// if (std::find(connected_users.begin(), connected_users.end(), id_connected) != connected_users.end())
		// {
		// 	shutdown(incoming_fd, SHUT_RDWR);
		// 	continue;
		// 	break;
		// }
		//connected_users.push_back(id_connected);		

		
		pid_t p = fork();
		if (!p)
		{
			int id = get_uid_from_socket(incoming_fd);
			cout << KMAG <<"Client " << id << " connected!" << KNRM << endl;
			add_new_user_in_storage(id);
			while (true)
			{
				package_message msg;
				vector<std::pair<string, bool>> res_list;
				res_list.clear();
				res = check(recv(incoming_fd, (void*)&msg, sizeof(msg), MSG_WAITALL));
				check(res);
				if (errno == ENOTCONN || res < sizeof(package_message))
					break;
				struct res_analist result_analist = analist_requets(msg, incoming_fd);

				switch (result_analist.req)
				{
					case REQ_OPEN:
						cout << "\n>> Client " << result_analist.uid << " send request to file " << result_analist.path_to_file << " of: " << result_analist.target_uid << endl;  
						cout << ">> Got right: " << convert_right_to_string(result_analist.right) << endl;
						if (result_analist.right >= 0)
						{	
							flag = get_flag_open_from_right(result_analist.right);
							fd_to_send = open(result_analist.path_to_file, flag, result_analist.mode);

							cout << "fd = " << fd_to_send << endl;
							if (fd_to_send < 0)
								result_analist.result_code = false;
							else
								result_analist.result_code = true;
							send_back_result_analist(result_analist, incoming_fd);
							send_descript(incoming_fd, fd_to_send);
						}
					break;

					case REQ_GET_RIGHTS:
						cout << "\n>> Client " << result_analist.uid << " send request check to file " << result_analist.path_to_file << " of: " << result_analist.target_uid << endl;  
						cout << ">> Got right: " << convert_right_to_string(result_analist.right) << endl;
						send_back_result_analist(result_analist, incoming_fd);
					break;

					case REQ_GET_STORAGE:
						cout<< "\nClient " << result_analist.uid << " send request get storage of user "
						 	<< result_analist.target_uid << endl;
						cout<< "Found storage: ";
						if (result_analist.result_code)
							cout << "True" << endl;
						else
							cout << "False" << endl;
						res_list = list_dir(result_analist.path_to_file);
						send_back_result_analist(result_analist, incoming_fd);
						send_storage(incoming_fd, res_list);
					break;

					case REQ_GRANT:
						cout<< "\nClient " << result_analist.uid << " send request to grant right " << result_analist.right << " to file" << result_analist.path_to_file 
							<< " for client: " << result_analist.target_uid << endl;
						cout<< "Granted? ";
						if (result_analist.result_code)
							cout << "True" << endl;
						else
							cout << "False" << endl;
						send_back_result_analist(result_analist, incoming_fd);
					break;
			
				}
			}
			cout << "Client " << id << " disconnected!" << endl;
			exit(0);
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
	read_setting_file();
	chdir(working_directory);

	struct sigaction act{};
	act.sa_flags = SA_NOCLDWAIT;
	sigaction(SIGCHLD, &act, NULL);

	//sleep(30);
	getcwd(cwd, sizeof(cwd));
	cout << cwd << endl;
	int listen_fd;
	create_socket(listen_fd);
	accept_client(listen_fd);


	return EXIT_SUCCESS;
}


