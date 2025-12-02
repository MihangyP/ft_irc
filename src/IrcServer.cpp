#include "IrcServer.hpp"

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
}

void	IrcServer::createSocket(void)
{
	int					status;
	struct sockaddr_in	addr;
	struct pollfd		poll;
	
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
	status = bind(_server_fd, (struct sockaddr *)&addr, sizeof addr);
	if (status == -1)
		IRC_EXCEPTION(strerror(errno));
	status = listen(_server_fd, BACKLOG);
	if (status == -1)
		IRC_EXCEPTION(strerror(errno));

	poll.fd = _server_fd;
	poll.events = POLLIN;
	poll.revents = 0;
	_fds.push_back(poll);
}
