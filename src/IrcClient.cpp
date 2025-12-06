#include "IrcClient.hpp"

IrcClient::IrcClient(int fd, std::string address)
{
	_fd = fd;
	_address = address;
	_is_connected = false;
	_authenticated = false;
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

bool	IrcClient::isConnected(void) const
{
	return (_is_connected);
}

std::vector<Command>	IrcClient::getAuthBuf(void) const
{
	return (_auth_buf);
}

bool	IrcClient::isAuthenticated(void) const
{
	return (_authenticated);
}

void		IrcClient::setIsConnected(bool status)
{
	_is_connected = status;
}

void		IrcClient::setAuthenticated(bool status)
{
	_authenticated = status;
}

void		IrcClient::pushIntoAuthBuf(Command cmd)
{
	_auth_buf.push_back(cmd);
}

void		IrcClient::clearAuthBuf(void)
{
	_auth_buf.clear();
}
