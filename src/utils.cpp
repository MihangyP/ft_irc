#include "ft_irc.h"
#include "IrcClient.hpp"
#include <cctype>
#include <algorithm>

// RFC 2812 Section 2.2: Case-insensitive string comparison for IRC
// IRC uses a special case mapping inherited from ASCII:
// - A-Z maps to a-z (standard lowercase)
// - [ maps to {
// - ] maps to }
// - \ maps to |
// - ^ maps to ~
// This ensures nicknames like [Test], {Test}, and {test} are all equivalent
std::string irc_tolower(const std::string& str)
{
	std::string result = str;
	for (size_t i = 0; i < result.size(); ++i) {
		unsigned char c = static_cast<unsigned char>(result[i]);
		// Standard A-Z to a-z conversion
		if (c >= 'A' && c <= 'Z') {
			result[i] = c + ('a' - 'A');
		}
		// IRC-specific case mappings (RFC 2812)
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

