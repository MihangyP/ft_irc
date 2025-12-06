#include "Command.hpp"

Command::Command(void)
{
 	// Absolutly nothing to put here
}

Command::Command(const Command &other)
{
	if (this != &other) {
		_command_name = other._command_name;
		_arguments = other._arguments;
	}
}

Command& Command::operator=(const Command &other)
{
	if (this != &other) {
		_command_name = other._command_name;
		_arguments = other._arguments;
	}
	return (*this);
}

Command::~Command(void)
{
 	// Absolutly nothing to put here
}

void	Command::setCommandName(std::string command_name)
{
	_command_name = command_name;
}

void	Command::setArguments(itr begin, itr end)
{
	for (; begin != end; ++begin) {
		_arguments.push_back(*begin);
	}
}

std::string					Command::getCommandName(void) const
{
	return (_command_name);
}

std::vector<std::string>	Command::getArguments(void) const
{
	return (_arguments);
}
