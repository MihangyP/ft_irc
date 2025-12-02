#include "IrcException.hpp"

std::string toString(int number)
{
	std::ostringstream	string_stream;

	string_stream << number;
	return (string_stream.str());
}

IrcException::IrcException(void)
{
	_message = "Default Error Message";
}

IrcException::IrcException(std::string message, std::string file, int line)
{
	_message = file + ": " + toString(line) + ": " + message;
}

IrcException::~IrcException(void) throw()
{
  // Nothing to put here
}

const char *IrcException::what(void) const throw()
{
	return (_message.c_str());
}
