#ifndef FUNCTION_TYPE_H
#define FUNCTION_TYPE_H

#include <iostream>
#include "type_packet.h"
using namespace std;



bool can_read(const right_t& r);
bool can_write(const right_t& r);
bool can_delete(const right_t& r);

bool is_less_right(const right_t& lhs, const right_t& rhs);
string convert_right_to_string(const right_t& r);

std::istream& operator>>(std::istream& is, right_t& r);

std::ostream& operator<<(std::ostream& os, const right_t& r);

int get_flag_open_from_right(right_t r);

#endif // FUNCTION_TYPE_H
