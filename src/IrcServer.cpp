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
	_client_try_to_authenticate = false;
	std::cout << "port: " << _port << std::endl;
	std::cout << "password: " << _password << std::endl;
}

IrcServer::~IrcServer(void)
{
	std::cout << "IrcServer Destructor Called!!" << std::endl;
}

// TODO: replace the vector of clients and fds into a map
void	IrcServer::addClient(void)
{
	struct sockaddr_in	addr;
	struct pollfd		poll;
	int					client_fd;
	socklen_t			addr_size = sizeof addr;

	client_fd = accept(_server_fd, (struct sockaddr *)&addr, &addr_size);
	if (client_fd == -1)
		IRC_EXCEPTION(strerror(errno));
	poll.fd = client_fd;
	poll.events = POLLIN;
	poll.revents = 0;

	std::string client_address = inet_ntoa(addr.sin_addr);

	IrcClient	client(client_fd, client_address);
	_clients.push_back(client);
	_fds.push_back(poll);
}

bool	IrcServer::tryToAuthenticate(int client_index)
{
	// NOTE: should we follow this order ?
	std::vector<Command>	auth_buf = _clients[client_index].getAuthBuf();
	if (auth_buf[0].getCommandName() == "PASS" && 
		auth_buf[1].getCommandName() == "NICK" &&
		auth_buf[2].getCommandName() == "USER") {
		return (true);	
	}
	_clients[client_index].clearAuthBuf();
	return (false);
}

void	IrcServer::parseReceivedData(std::string message, int fd)
{
	// Get the actual client corresponding to fd
	// i is the corresponding client index in _clients
	size_t i = 0;
	for (; i < _clients.size(); ++i) {
		if (_clients[i].getFd() == fd)
			break ;
	}
	if (i == _clients.size())
		return ;
	//

	Command	cmd;
	std::string	valid_command_names[] = {
		"PASS",
		"NICK",
		"USER",
	};
	size_t valid_command_names_size = sizeof(valid_command_names) / sizeof(valid_command_names[0]);

	StringHelper	tmp(message);
	std::vector<std::string>	tokens = tmp.trim().splitBySpace();

	bool	found = false;
	for (size_t i = 0; i < valid_command_names_size; ++i) {
		if (tokens[0] == valid_command_names[i]) {
			found = true;
			break ;
		}
	}

	if (found) {
		cmd.setCommandName(tokens[0]);
	} else {
		std::cout << "Invalid command name" << std::endl;
	}
	cmd.setArguments(tokens.begin() + 1, tokens.end());
	// TODO: check error for each command
	if (!_clients[i].isConnected()) {
		_clients[i].pushIntoAuthBuf(cmd);
		std::vector<Command>	auth_buf = _clients[i].getAuthBuf();
		if (auth_buf.size() == 3) {
			std::cout << "YOO\n";
			if (tryToAuthenticate(i)) {
				_clients[i].setAuthenticated(true);
			}
		}

		if (_clients[i].isAuthenticated()) {
			std::cout << "TRY TO AUTHENTICATE" << std::endl;
			std::vector<Command>	auth_buf = _clients[i].getAuthBuf();
			std::vector<std::string> arguments = auth_buf[0].getArguments();
			if (arguments.empty())
				return ;
			if (arguments[0] == _password) { // if the password is correct
				_clients[i].setIsConnected(true);
				std::cout << "Client " << _clients[i].getFd() << " Connected!" << std::endl;
			} else {
				std::cout << "invalid password" << std::endl;
			}
			if (_clients[i].isConnected()) {
				_clients[i].setAuthenticated(false);
			}
		}
	}

	//std::cout << "cmd_name: " << cmd.getCommandName() << std::endl;
	//std::vector<std::string>	arguments = cmd.getArguments();
	//std::cout << "_arguments: " << std::endl;
	//for (size_t i = 0; i < arguments.size(); ++i) {
		//std::cout << arguments[i] << std::endl;
	//}
}

void	IrcServer::readData(int fd)
{
	// TODO : make this dynamic ?
	# define MAX_MESSAGE_SIZE 1024
	char	message[MAX_MESSAGE_SIZE];
	memset(message, 0, sizeof message);

	ssize_t	bytes_read = recv(fd, message, sizeof message - 1, 0);
	if (bytes_read > 0) {
		message[bytes_read] = '\0';
		std::cout << "Client " << fd << ", Data: " << message << std::endl;
		parseReceivedData(message, fd);
	} else {
		for (size_t i = 0; i < _clients.size(); ++i) {
			if (_clients[i].getFd() == fd) {
				_clients.erase(_clients.begin() + i);
				break ;
			}
		}
		for (size_t i = 0; i < _fds.size(); ++i) {
			if (_fds[i].fd == fd) {
				_fds.erase(_fds.begin() + i);
				break ;
			}
		}
		close(fd);
		std::cout << "Client " << fd << " disconnected" << std::endl;
	}
}

void	IrcServer::init(void)
{
	createSocket();
	std::cout << "IrcSever " << _server_fd << " Connected" << std::endl;

	// event loop
	while (!IrcServer::signal) {
		int	status;
		status = poll(&_fds[0], _fds.size(), -1);
		if (status == -1 && !IrcServer::signal)
			IRC_EXCEPTION(strerror(errno));
		for (size_t i = 0; i < _fds.size(); ++i) {
			if (_fds[i].revents & POLLIN) {
				if (_fds[i].fd == _server_fd) {
					addClient();
				} else {
					readData(_fds[i].fd);
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
	status = fcntl(_server_fd, F_SETFL, O_NONBLOCK);
	if (status == -1)
		IRC_EXCEPTION(strerror(errno));
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
	for (size_t i = 0; i < _clients.size(); ++i) {
		int	client_fd = _clients[i].getFd();
		close(client_fd);
	}
	if (_server_fd != -1)
		close(_server_fd);
}
