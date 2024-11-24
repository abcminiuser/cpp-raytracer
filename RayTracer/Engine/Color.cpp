#include "Engine/Color.hpp"

#include <cstdio>
#include <iterator>

std::string	Color::string() const
{
	char buffer[32];
	snprintf(buffer, std::size(buffer), "(%f, %f, %f)", m_red, m_green, m_blue);
	buffer[std::size(buffer) - 1] = '\0';

	return buffer;
}
