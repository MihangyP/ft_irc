#include "ft_irc.h"
#include "IrcClient.hpp"
#include <cctype>
#include <algorithm>

std::string irc_tolower(const std::string& str)
{
	std::string result = str;
	for (size_t i = 0; i < result.size(); ++i) {
		unsigned char c = static_cast<unsigned char>(result[i]);
		if (c >= 'A' && c <= 'Z') {
			result[i] = c + ('a' - 'A');
		}
		else if (c == '[') {
			result[i] = '{';
		}
		else if (c == ']') {
			result[i] = '}';
		}
		else if (c == '\\') {
			result[i] = '|';
		}
		else if (c == '^') {
			result[i] = '~';
		}
	}
	return result;
}

bool irc_iequal(const std::string& a, const std::string& b)
{
	return irc_tolower(a) == irc_tolower(b);
}

