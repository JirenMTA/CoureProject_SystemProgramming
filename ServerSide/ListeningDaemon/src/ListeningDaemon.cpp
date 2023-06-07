#include <sys/socket.h>
#include <syslog.h>
#include <signal.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <StorageLib/sec_library.h>
#include "functions_of_types.h"
#include "common.h"
#include <sstream>
using namespace std;
/*
sudo useradd -m -G adm,sdrom,sudo -s /bin/bash SEC_OPERATOR
id -u <username>
tail -n 10 /var/log/syslog      // to read log-file (message from daemon) (10 last row in file)
grep <whatfind> /var/log/syslog // to read log-file (message from daemon)
ps -jx | grep <program's name>    // to list all running daemon in terminal
cp  listening_daemon.service /etc/systemd/system
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
	for (x = sysconf(_SC_OPEN_MAX); x >= 0; x--)
	{
		close(x);
	}
	openlog("Server_Daemon", LOG_PID, LOG_DAEMON);
}

void accept_client(int &listen_fd)
{
	int flag = 1;
	int res = 1;
	int fd_to_send = 1;
	int incoming_fd = 1;
	int id = 0;
	syslog(LOG_NOTICE, "Started listening");
	while (true)
	{
		socklen_t incoming_len;
		sockaddr_un incoming_address;
		string str_right;
		int result_delete;
		incoming_fd = accept(listen_fd, (sockaddr *)&incoming_address, &incoming_len);
		stringstream msg_log;
		pid_t p = fork();
		if (!p)
		{
			id = get_uid_from_socket(incoming_fd);
			cout << "Client " << id << " connected!"<< endl;
			msg_log << "Client " << id << " connected!" << endl;
			syslog(LOG_NOTICE, "%s", msg_log.str().c_str());
			msg_log.str("");
			add_new_user_in_storage(id);
			while (true)
			{
				struct request msg{};
				vector<std::pair<string, bool>> res_list;
				res_list.clear();
				res = check(recv(incoming_fd, (void *)&msg, sizeof(msg), MSG_WAITALL));
				check(res);
				if (errno == ENOTCONN || res < sizeof(request))
					break;
				struct response result_analist = analist_requets(msg, incoming_fd);

				switch (result_analist.req)
				{
				case REQ_OPEN:
					
					cout << "\n>> Client " << result_analist.uid << " send request to file "
                    << result_analist.path_to_file << " of: " << result_analist.target_uid << endl;
					cout << ">> Got right: " << convert_right_to_string(result_analist.right) << endl;
					msg_log << "Client " << result_analist.uid << " send request to file "
                    << result_analist.path_to_file << " of: " << result_analist.target_uid << endl;
					syslog(LOG_NOTICE, "%s", msg_log.str().c_str());
					msg_log.str("");
					msg_log << "Got right: " << convert_right_to_string(result_analist.right) << endl;	
					syslog(LOG_NOTICE, "%s", msg_log.str().c_str());				
					msg_log.str("");
					if (result_analist.right >= 0)
					{
						if (result_analist.uid == result_analist.target_uid)
						{
							fd_to_send = open(result_analist.path_to_file, O_RDWR | O_CREAT, result_analist.mode);
						}
						else
						{
							flag = get_flag_open_from_right(result_analist.right);
							fd_to_send = open(result_analist.path_to_file, flag, result_analist.mode);
						}
					}
					cout << "fd = " << fd_to_send << endl;
					msg_log << "fd = " << fd_to_send << endl;
					syslog(LOG_NOTICE, "%s", msg_log.str().c_str());				
					msg_log.str("");
					if (fd_to_send < 0)
						result_analist.result_code = false;
					else
						result_analist.result_code = true;
					send_back_result_analist(result_analist, incoming_fd);
					send_descript(incoming_fd, fd_to_send);
					break;

				case REQ_GET_RIGHTS:
					cout << "\n>> Client " << result_analist.uid << " send request check to file "
                    << result_analist.path_to_file << " of: " << result_analist.target_uid << endl;
					cout << ">> Got right: " << convert_right_to_string(result_analist.right) << endl;
					
					msg_log << ">> Client " << result_analist.uid << " send request check to file "
                    << result_analist.path_to_file << " of: " << result_analist.target_uid << endl;
					syslog(LOG_NOTICE, "%s", msg_log.str().c_str());	
					msg_log.str("");	
								
					msg_log << ">> Got right: " << convert_right_to_string(result_analist.right) << endl;	
					syslog(LOG_NOTICE, "%s", msg_log.str().c_str());	
					msg_log.str("");
								
					send_back_result_analist(result_analist, incoming_fd);
					break;

				case REQ_GET_STORAGE:
					cout << "\nClient " << result_analist.uid << " send request get storage of user "
						 << result_analist.target_uid << endl;
					msg_log << "Client " << result_analist.uid << " send request get storage of user "
                    << result_analist.target_uid << endl;
					syslog(LOG_NOTICE, "%s", msg_log.str().c_str());
					msg_log.str("");
					syslog(LOG_NOTICE, "%s", msg_log.str().c_str());	
					msg_log.str("");
					cout << "Found storage? " << endl;
					if (result_analist.result_code)
						cout << "True" << endl;
					else
						cout << "False" << endl;
					res_list = list_dir(result_analist.path_to_file);
					send_back_result_analist(result_analist, incoming_fd);
					send_storage(incoming_fd, res_list);
					break;

				case REQ_GRANT:
					cout << "\nClient " << result_analist.uid << " send request to grant right "
                    << result_analist.right << " to file" << result_analist.path_to_file
						 << " for client: " << result_analist.target_uid << endl;
					msg_log << "Client " << result_analist.uid << " send request to grant right "
                    << result_analist.right << " to file" << result_analist.path_to_file
						 << " for client: " << result_analist.target_uid << endl;
					syslog(LOG_NOTICE, "%s", msg_log.str().c_str());	
					msg_log.str("");
	 
					msg_log << "Granted? ";
					if (result_analist.result_code)
						msg_log << "True" << endl;
					else
						msg_log << "False" << endl;
					syslog(LOG_NOTICE, "%s", msg_log.str().c_str());	
					msg_log.str("");
											
					send_back_result_analist(result_analist, incoming_fd);
					break;
				case REQ_REVOKE:
					cout << "\n>> Client " << result_analist.uid << " revoke right of file " 
						 << "\'"<< result_analist.path_to_file << "\' "
						 <<  "from user " << result_analist.target_uid << endl;
					msg_log << "\n>> Client " << result_analist.uid << " revoke right of file " 
						 << "\'"<< result_analist.path_to_file << "\' "
						 <<  "from user " << result_analist.target_uid << endl; 
					syslog(LOG_NOTICE, "%s", msg_log.str().c_str());	
					msg_log.str("");
						 
					msg_log << ">> Result revoke: " << result_analist.right << endl;	
					syslog(LOG_NOTICE, "%s", msg_log.str().c_str());	
					msg_log.str("");
					
					send_back_result_analist(result_analist, incoming_fd);
					break;
				case REQ_UNLINK:
					cout << "\n>> Client " << result_analist.uid << " send request to delete file " 
						 << "\'"<< result_analist.path_to_file << "\' "
						 <<  "from user " << result_analist.target_uid << endl;
					msg_log  << ">> Client " << result_analist.uid << " send request to delete file " 
						 << "\'"<< result_analist.path_to_file << "\' "
						 <<  "from user " << result_analist.target_uid << endl;
					syslog(LOG_NOTICE, "%s", msg_log.str().c_str());	
					msg_log.str("");
					
					msg_log << ">> Deleted? ";
					if (result_analist.result_code)
						msg_log << "Yes" << endl;
					else
						msg_log << "No" << endl;
					syslog(LOG_NOTICE, "%s", msg_log.str().c_str());	
					msg_log.str("");
					
					send_back_result_analist(result_analist, incoming_fd);
					break;

                    case REQ_BAN_USER:
                        cout << "\n>> Client " << result_analist.uid << " send request ban user to file "
                             << "\'"<< result_analist.path_to_file << "\' "
                             <<  "from user " << result_analist.target_uid << endl;
                        msg_log  << ">> Client " << result_analist.uid << " send request ban user to file "
                                 << "\'"<< result_analist.path_to_file << "\' "
                                 <<  "from user " << result_analist.target_uid << endl;
                        syslog(LOG_NOTICE, "%s", msg_log.str().c_str());
                        msg_log.str("");

                        msg_log << ">> Ban ? ";
                        if (result_analist.result_code)
                            msg_log << "Yes" << endl;
                        else
                            msg_log << "No" << endl;
                        syslog(LOG_NOTICE, "%s", msg_log.str().c_str());
                        msg_log.str("");

                        send_back_result_analist(result_analist, incoming_fd);
                        break;
                    case REQ_UNBAN_USER:
                        cout << "\n>> Client " << result_analist.uid << " send request unban user to file "
                             << "\'"<< result_analist.path_to_file << "\' "
                             <<  "from user " << result_analist.target_uid << endl;
                        msg_log  << ">> Client " << result_analist.uid << " send request unban user to file "
                                 << "\'"<< result_analist.path_to_file << "\' "
                                 <<  "from user " << result_analist.target_uid << endl;
                        syslog(LOG_NOTICE, "%s", msg_log.str().c_str());
                        msg_log.str("");

                        msg_log << ">> Unban ? ";
                        if (result_analist.result_code)
                            msg_log << "Yes" << endl;
                        else
                            msg_log << "No" << endl;
                        syslog(LOG_NOTICE, "%s", msg_log.str().c_str());
                        msg_log.str("");

                        send_back_result_analist(result_analist, incoming_fd);
                        break;

                    case REQ_SET_PASSWD:
                        cout << "\n>> Client " << result_analist.uid << " send request set passwd to file "
                             << "\'"<< result_analist.path_to_file << "\' "
                             <<  "from user " << result_analist.target_uid << endl;
                        msg_log  << ">> Client " << result_analist.uid << " send request set passwd to file "
                                 << "\'"<< result_analist.path_to_file << "\' "
                                 <<  "from user " << result_analist.target_uid << endl;
                        syslog(LOG_NOTICE, "%s", msg_log.str().c_str());
                        msg_log.str("");

                        msg_log << ">> Set passwd ? ";
                        if (result_analist.result_code)
                            msg_log << "Yes" << endl;
                        else
                            msg_log << "No" << endl;
                        syslog(LOG_NOTICE, "%s", msg_log.str().c_str());
                        msg_log.str("");

                        send_back_result_analist(result_analist, incoming_fd);
                        break;
                    case REQ_AUTH_BY_PASSWD:
                        cout << "\n>> Client " << result_analist.uid << " send request authorization "
                             << "\'"<< result_analist.path_to_file << "\' "
                             <<  "from user " << result_analist.target_uid << endl;
                        msg_log  << ">> Client " << result_analist.uid << " send request authorization "
                                 << "\'"<< result_analist.path_to_file << "\' "
                                 <<  "from user " << result_analist.target_uid << endl;
                        syslog(LOG_NOTICE, "%s", msg_log.str().c_str());
                        msg_log.str("");

                        msg_log << ">> Authorization ? ";
                        if (result_analist.result_code)
                            msg_log << "Yes" << endl;
                        else
                            msg_log << "No" << endl;
                        syslog(LOG_NOTICE, "%s", msg_log.str().c_str());
                        msg_log.str("");

                        send_back_result_analist(result_analist, incoming_fd);
                        break;
                    case REQ_EXIST_PASSWD:
                        cout << "\n>> Client " << result_analist.uid << " send request exist passwd "
                             << "\'"<< result_analist.path_to_file << "\' "
                             <<  "from user " << result_analist.target_uid << endl;
                        msg_log  << ">> Client " << result_analist.uid << " send request exist passwd "
                                 << "\'"<< result_analist.path_to_file << "\' "
                                 <<  "from user " << result_analist.target_uid << endl;
                        syslog(LOG_NOTICE, "%s", msg_log.str().c_str());
                        msg_log.str("");

                        msg_log << ">> Exist passwd ? ";
                        if (result_analist.result_code)
                            msg_log << "Yes" << endl;
                        else
                            msg_log << "No" << endl;
                        syslog(LOG_NOTICE, "%s", msg_log.str().c_str());
                        msg_log.str("");

                        send_back_result_analist(result_analist, incoming_fd);
                        break;
                    case REQ_USER_IN_BAN:
                        cout << "\n>> Client " << result_analist.uid << " send request user in ban "
                             << "\'"<< result_analist.path_to_file << "\' "
                             <<  "from user " << result_analist.target_uid << endl;
                        msg_log  << ">> Client " << result_analist.uid << " send request user in ban "
                                 << "\'"<< result_analist.path_to_file << "\' "
                                 <<  "from user " << result_analist.target_uid << endl;
                        syslog(LOG_NOTICE, "%s", msg_log.str().c_str());
                        msg_log.str("");

                        msg_log << ">> User in ban ? ";
                        if (result_analist.result_code)
                            msg_log << "Yes" << endl;
                        else
                            msg_log << "No" << endl;
                        syslog(LOG_NOTICE, "%s", msg_log.str().c_str());
                        msg_log.str("");

                        send_back_result_analist(result_analist, incoming_fd);
                        break;
                }
			}
			cout << "Client " << id << " disconnected!" << endl;
			msg_log << "Client " << id << " disconnected!" << endl;
			syslog(LOG_NOTICE, "%s", msg_log.str().c_str());	
			msg_log.str("");
			
			exit(0);
		}
	}
}


int main()
{
	skeleton_daemon();
	char cwd[64];
	struct passwd *pass;
	read_setting_file();
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
