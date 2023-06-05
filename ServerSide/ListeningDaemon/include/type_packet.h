#ifndef TYPE_PACKET_H
#define TYPE_PACKET_H

#include <unistd.h>
#include <fcntl.h>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

enum TYPE
{
	REQ_OPEN,
	REQ_GET_RIGHTS,
	REQ_UNLINK,
	REQ_OPENAT,
	REQ_GRANT,
	REQ_REVOKE,
    REQ_CREATE,
	REQ_GET_STORAGE,
    REQ_BAN_USER,
    REQ_SET_PASSWD
}; 

enum right_t
{
	R_NONE   = 0,
	R_READ   = 1,
	R_WRITE  = 2,
	R_RDWR   = 3,
	R_DELETE = 4,
	R_RDDEL  = 5,
	R_WRDEL  = 6,
	R_ALL    = 7
};

struct request
{
	TYPE req;
	int target_id;
	char filename[32]={'\0'};	
	mode_t mode;
	right_t right;

    char passwd[32]={'\0'};
};

struct response
{
	TYPE req;
	right_t right;
	mode_t mode;
	int uid;
	int target_uid;
	char path_to_file[48]={'\0'};
	bool result_code;
};

struct line_right{
	int uid;
	char filename[32]={'\0'};
	int right;
};

#endif // TYPE_PACKET_H






















