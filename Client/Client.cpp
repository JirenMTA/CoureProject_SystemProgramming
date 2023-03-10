#include <iostream>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include "TestLib.cpp"
using namespace std;

int main()
{
	int choice;
	
	sec_init();
	while (true)
	{
		cout << "1. Read		2. Write	3. Get right	4. Get list file" << endl
			 << "5. Grant	6. List storage" << endl;
		cin >> choice;
		switch (choice)
		{
			case 1:
				task_try_read();
				break;
			case 2:
				task_try_write();
				break;
			case 3: 
				task_get_right();
				break;
			case 4:
				task_try_get_storage();
				break;
			case 5:
				task_try_grant();
				break;
			case 6: 
				task_try_list_storage();
				break;
		}
	}
	return 0;
}


 