#include "IrcServer.hpp"

bool IrcServer::signal = false;

void	IrcServer::signalHandler(int signum)
{
	(void)signum;
	std::cout << std::endl;
	std::cout << "signal received!!!!!" << std::endl;
	IrcServer::signal = true;
}

IrcServer::IrcServer(size_t port, std::string password)
{
	std::cout << "IrcServer Default Constructor Called!!" << std::endl;
	_server_fd = -1;
	_port = port;
	_password = password;
	std::cout << "port: " << _port << std::endl;
	std::cout << "password: " << _password << std::endl;
}

IrcServer::IrcServer(const IrcServer& other)
{
	std::cout << "IrcServer Copy Constructor Called!!" << std::endl;
	if (this != &other) {
		_server_fd = other._server_fd;
		_port = other._port;
		_password = other._password;
	}
}

IrcServer& IrcServer::operator=(const IrcServer &other)
{
	std::cout << "IrcServer Assignment Operator Called!!" << std::endl;
	if (this != &other) {
		_server_fd = other._server_fd;
		_port = other._port;
		_password = other._password;
	}
	return (*this);
}

IrcServer::~IrcServer(void)
{
	std::cout << "IrcServer Destructor Called!!" << std::endl;
}

void	IrcServer::init(void)
{
	createSocket();
	std::cout << "IrcSever " << _server_fd << " Connected" << std::endl;

	while (!IrcServer::signal) {
		int	status;
		status = poll(&_fds[0], _fds.size(), -1);
		if (status == -1 && !IrcServer::signal)
			IRC_EXCEPTION(strerror(errno));
		for (size_t i = 0; i < _fds.size(); ++i) {
			if (_fds[i].revents & POLLIN) {
				if (_fds[i].fd == _server_fd) {
					// TODO: accept new connection
				} else {
					// TODO: receive data
				}
			}
		}
	}
	closeFds();
}

void	IrcServer::createSocket(void)
{
	int					status;
	struct sockaddr_in	addr;
	struct pollfd		new_poll;
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_port);
	addr.sin_addr.s_addr = INADDR_ANY;

	_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (_server_fd == -1)
		IRC_EXCEPTION(strerror(errno));
	int opt = 1;
	status = setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);
	if (status == -1)
		IRC_EXCEPTION(strerror(errno));
	// TODO: is fcntl necessary ?
	//status = fcntl(_server_fd, F_SETFL, O_NONBLOCK);
	//if (status == -1)
		//IRC_EXCEPTION(strerror(errno));
	status = bind(_server_fd, (struct sockaddr *)&addr, sizeof addr);
	if (status == -1)
		IRC_EXCEPTION(strerror(errno));
	status = listen(_server_fd, BACKLOG);
	if (status == -1)
		IRC_EXCEPTION(strerror(errno));

	new_poll.fd = _server_fd;
	new_poll.events = POLLIN;
	new_poll.revents = 0;
	_fds.push_back(new_poll);
}

void	IrcServer::closeFds(void)
{
	// TODO: close client fds
	if (_server_fd != -1)
		close(_server_fd);
}
