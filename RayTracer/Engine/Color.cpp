#include "Engine/Color.hpp"

#include <iterator>
#include <stdio.h>

std::string	Color::string() const
{
	char buffer[32];
	snprintf(buffer, std::size(buffer), "(%u, %u, %u)", m_red, m_green, m_blue);
	buffer[std::size(buffer) - 1] = '\0';

	return buffer;
}
