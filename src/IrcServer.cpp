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
	std::cout << "Sever " << "Connected" << std::endl;
}

void	IrcServer::createSocket(void)
{
	struct sockaddr_in addr;
	
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_port);
	addr.sin_addr.s_addr = INADDR_ANY;

	_server_fd = socket(AF_INET, SOCK_STREAM, 0);
	//if (_server_fd == -1)
		//IRC_EXCEPTION("Hello");
}
