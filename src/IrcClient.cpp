#include "IrcClient.hpp"

IrcClient::IrcClient(int fd, std::string address)
{
	_fd = fd;
	_address = address;
	_nickname = "";
	_username = "";
	input_buffer = "";
	authenticated = false;
	registered = false;
}

IrcClient::~IrcClient(void)
{
	// Nothing to put here
}

int	IrcClient::getFd(void) const
{
	return (_fd);
}

std::string	IrcClient::getAddress(void) const
{
	return (_address);
}

void		IrcClient::setNickName(std::string nickname)
{
	_nickname = nickname;
}

void		IrcClient::setUserName(std::string username)
{
	_username = username;
}

std::string	IrcClient::getNickName(void) const
{
	return (_nickname);
}

std::string	IrcClient::getUserName(void) const
{
	return (_username);
}

void	IrcClient::addChannel(Channel chan)
{
	channels.push_back(chan);
}

void	IrcClient::quitAllChannels(void)
{
	channels.clear();	
}
