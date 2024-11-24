#include "Engine/Vector.hpp"

#include <cstdio>
#include <iterator>

std::string	Vector::string() const
{
	char buffer[64];
	snprintf(buffer, std::size(buffer), "(%+f, %+f, %+f)", m_x, m_y, m_z);
	buffer[std::size(buffer) - 1] = '\0';

	return buffer;
}
