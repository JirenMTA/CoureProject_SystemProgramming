#include <iostream>
#include "functions_of_types.h"
using namespace std;

bool can_read(const right_t& r)
{
	return (r == R_READ|| r == R_RDWR|| r == R_RDDEL|| r == R_ALL);
}
bool can_write(const right_t& r)
{
	return (r == R_WRITE|| r == R_RDWR || r == R_ALL || r == R_WRDEL);
}
bool can_delete(const right_t& r)
{
	return (r == R_WRDEL || r == R_RDDEL || r == R_ALL || r == R_DELETE);
}

bool is_less_right(const right_t& lhs, const right_t& rhs)
{
	if (can_read(lhs) && !can_read(rhs))
		return false;
	if (can_write(lhs) && !can_write(rhs))
		return false;
	if (can_delete(lhs) && !can_delete(rhs))
		return false;
	return true;
}

string convert_right_to_string(const right_t& r)
{
	switch (r)
	{
		case R_NONE:
			return "R_NONE";
		case R_READ :
			return "R_READ";
		case R_WRITE:
			return "R_WRITE";
		case R_RDDEL:
			return "R_RDDEL";
		case R_WRDEL:
			return "R_WRDEL";
		case R_RDWR:
			return "R_RDWR";
		case R_ALL:
			return "R_ALL";
		case R_DELETE:

			return "R_DELETE";
		default:
			return "R_NONE";
	}
}

int get_flag_open_from_right(right_t r)
{
	if (can_read(r) && can_write(r))
		return O_RDWR;
	if (can_read(r))
		return O_RDONLY;
	if (can_write(r))
		return O_WRONLY;
	return -1; 
}
