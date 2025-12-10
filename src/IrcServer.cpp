#include "IrcServer.hpp"

bool IrcServer::signal = false;

void	IrcServer::signalHandler(int signum)
{
	std::cout << std::endl;
	IrcLog::info("receive signal %i", signum);
	IrcServer::signal = true;
}

IrcServer::IrcServer(size_t port, std::string password)
{
	_server_fd = -1;
	_port = port;
	_password = password;
	IrcLog::info("port = %i", _port);
	IrcLog::info("password = %s", _password.c_str());
}

IrcServer::~IrcServer(void)
{

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
	int status = fcntl(client_fd, F_SETFL, O_NONBLOCK);
	if (status == -1)
		IRC_EXCEPTION(strerror(errno));
	poll.fd = client_fd;
	poll.events = POLLIN;
	poll.revents = 0;

	std::string client_address = inet_ntoa(addr.sin_addr);

	IrcClient	client(client_fd, client_address);
	_clients.push_back(client);
	_fds.push_back(poll);
}

void sendMessage(const IrcClient& client, const std::string& message)
{
	send(client.getFd(), message.c_str(), message.size(), 0);
}

void	IrcServer::disconnectClient(int fd)
{
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
}

void	IrcServer::tryToRegister(int client_index)
{
	if (_clients[client_index].authenticated &&
		_clients[client_index].getNickName() != "" &&
		_clients[client_index].getUserName() != "") {
		_clients[client_index].registered = true;
		std::string response = ":ft_irc 001 " + _clients[client_index].getNickName() + " :Welcome to FT_IRC!\r\n";
		sendMessage(_clients[client_index], response);
		IrcLog::info("Client %i registered as %s (username: %s)", _clients[client_index].getFd(),
				_clients[client_index].getNickName().c_str(),
				_clients[client_index].getUserName().c_str());
	}
}

t_command	getAppropriateTag(const std::string& command_name)
{
	if (command_name == "PASS") return (PASS);
	else if (command_name == "NICK") return (NICK);
	else if (command_name == "USER") return (USER);
	else return (UNKNOWN);
}

void	IrcServer::handleCommand(Command cmd, int client_index)
{
	t_command command_tag = getAppropriateTag(cmd.getCommandName());
	std::vector<std::string> arguments = cmd.getArguments();

	if (command_tag == PASS) {
		if (arguments[0] == _password) {
			_clients[client_index].authenticated = true;
		} else {
			// TODO: rewrite this properly
			std::string response = ":ft_irc 464 " + _clients[client_index].getNickName() + " :Password incorrect!\r\n";
			sendMessage(_clients[client_index], response);
			return ;
		}
	} else if (command_tag == NICK) {
		_clients[client_index].setNickName(arguments[0]);
	} else if (command_tag == USER) {
		_clients[client_index].setUserName(arguments[0]);
	}

	if (!_clients[client_index].registered) {
		tryToRegister(client_index);
	}
}

void	IrcServer::parseCommand(std::string line, int client_index)
{
	Command cmd;

	std::string status = ParseCommand::parseCmd(line, cmd, _password, _clients, client_index);
	if (status == EMPTY_COMMAND) return ; // Ignore
	else if (status == ERR_NEEDMOREPARAMS) {
		//std::string	response = ":"SERVER_NAME"";	
	} else if (status == ERR_PASSDMISMATCH) {
		disconnectClient(_clients[client_index].getFd());
		///
	} else if (status == SUCCESS) {
		handleCommand(cmd, client_index);
	}
}

void	IrcServer::readData(int fd)
{
	char	message[MAX_MESSAGE_SIZE];
	memset(message, 0, sizeof message);

	ssize_t	bytes_read = recv(fd, message, sizeof message - 1, 0);
	if (bytes_read > 0) {
		message[bytes_read] = '\0';
		std::cout << "Client " << fd << ", Data: " << message << std::endl;
		size_t i;
		for (i = 0; i < _clients.size(); ++i) {
			if (_clients[i].getFd() == fd)
				break ;
		}
		_clients[i].input_buffer.append(message);
		size_t pos = _clients[i].input_buffer.find("\n");
		while (pos != std::string::npos) {
			std::string line = _clients[i].input_buffer.substr(0, pos);
			if (!line.empty() && line[line.size() - 1] == '\r')
				line.erase(line.end() - 1);
			_clients[i].input_buffer.erase(0, pos + 1);
			parseCommand(line, i);
			pos = _clients[i].input_buffer.find("\n");
		}
	} else {
		disconnectClient(fd);
		IrcLog::info("Client [%i] disconnected", fd);
	}
}

void	IrcServer::init(void)
{
	createSocket();
	IrcLog::info("IrcServer [%i] connected", _server_fd);

	// event loop
	while (!IrcServer::signal) {
		int	status;
		status = poll(&_fds[0], _fds.size(), -1);
		if (status == -1 && !IrcServer::signal) // TODO: or just continue ?
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
	// TODO: is fcntl necessary ? // it suppose to make the fd no-bloquing
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
