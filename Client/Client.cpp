#include <iostream>
#include "TestLib.cpp"

using namespace std;

int main()
{
	int choice;
	sec_init();
	while (true)
	{
		cout << "\n1. Read		2. Write	3. Get right	4. Get list file" << endl
			 << "5. Grant	6. Rewoke	7. Delete" << endl;
        std::cout << "8. Ban user       9. Unban user      10. Passwd file" << std::endl;
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
				task_try_list_storage();
				break;
			case 5:
				task_try_grant();
				break;	
			case 6:
				task_try_revoke();
				break;	
			case 7:
				task_try_delete();
				break;
            case 8:
                task_try_ban_user();
                break;
            case 9:
                task_try_unban_user();
                break;
            case 10:
                task_passwd_by_file();
                break;
        }
	}
	return 0;
}