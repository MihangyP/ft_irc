#include "Channel.hpp"

Channel::Channel(std::string chan_name)
{
	_name = chan_name;
}

Channel::~Channel(void)
{
	// Nothing to put here
}

std::string	Channel::getName(void) const
{
	return (_name);
}
