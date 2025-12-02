#include "IrcClient.hpp"

IrcClient::IrcClient(int fd, std::string address)
{
	_fd = fd;
	_address = address;
}

IrcClient::IrcClient(const IrcClient& other)
{
	if (this != &other) {
		_fd = other._fd;
		_address = other._address;
	}
}

IrcClient& IrcClient::operator=(const IrcClient &other)
{
	if (this != &other) {
		_fd = other._fd;
		_address = other._address;
	}
	return (*this);
}

IrcClient::~IrcClient(void)
{
	// TODO: don't forget to close fd
	close(_fd);
}

int	IrcClient::getFd(void) const
{
	return (_fd);
}

std::string	IrcClient::getAddress(void) const
{
	return (_address);
}
