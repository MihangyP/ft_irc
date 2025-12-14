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

std::vector<Channel>	IrcServer::getAvailableChannels(void) const
{
	return (_available_channels);
}

void	IrcServer::addIntoAvailableChannels(Channel channel)
{
	_available_channels.push_back(channel);
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
	IrcLog::info("Client [%i] disconnected", fd);
}

void	IrcServer::tryToRegister(int client_index)
{
	if (_clients[client_index].authenticated &&
		_clients[client_index].getNickName() != "" &&
		_clients[client_index].getUserName() != "") {
		_clients[client_index].registered = true;
		{
			//:server 001 <nick> :Welcome to the IRC Network <nick>!<user>@<host>
			std::string response = ":" + std::string(SERVER_NAME) +
				" " + std::string(RPL_WELCOME) +
				" " + _clients[client_index].getNickName() + " :Welcome to FT_IRC! " +
				_clients[client_index].getNickName() + "!" + _clients[client_index].getUserName() +
				"@localhost\r\n";
			sendMessage(_clients[client_index], response);
		}
		{
			std::string response = ":" + std::string(SERVER_NAME) +
				" " + std::string(RPL_YOURHOST) +
				" " + _clients[client_index].getNickName()  + " :Your host is " +
				std::string(SERVER_NAME) + "\r\n";
			sendMessage(_clients[client_index], response);
		}
		{
			std::string response = ":" + std::string(SERVER_NAME) +
				" " + std::string(RPL_CREATED) +
				" " + _clients[client_index].getNickName()  + " :This server was created at ...\r\n";
			sendMessage(_clients[client_index], response);
		}
		{
			//:server 004 <nick> <server> 1.0 oiwsz biklmnopstvr
			std::string response = ":" + std::string(SERVER_NAME) +
				" " + std::string(RPL_MYINFO) +
				" " + _clients[client_index].getNickName() +
				" " + std::string(SERVER_NAME) +
				" 1.0 My Info\r\n";
			sendMessage(_clients[client_index], response);
		}
		IrcLog::info("Client %i registered as %s (username: %s)", _clients[client_index].getFd(),
				_clients[client_index].getNickName().c_str(),
				_clients[client_index].getUserName().c_str());
	}
}

t_command	commandNameToTag(const std::string& command_name)
{
	if (command_name == "PASS") return (PASS);
	else if (command_name == "NICK") return (NICK);
	else if (command_name == "USER") return (USER);
	else if (command_name == "QUIT") return (QUIT);
	else if (command_name == "CAP") return (CAP);
	else if (command_name == "PRIVMSG") return (PRIVMSG);
	else if (command_name == "JOIN") return (JOIN);
	else return (UNKNOWN);
}

size_t 	IrcServer::getCorrespondingClient(std::string nickname)
{
	size_t	client_index = 0;
	for (size_t i = 0; i < _clients.size(); ++i) {
		if (_clients[i].getNickName() == nickname) {
			client_index = i;
			break;
		}
	}
	return (client_index);
}

void	IrcServer::handleJoinCommand(Command cmd, int client_index)
{
	std::vector<std::string> arguments = cmd.getArguments();
	std::string response;

	StringHelper sh(arguments[0]);
	std::vector<std::string> channels = sh.trim().splitByDelimiter(',');
	if (channels.size() == 1 && channels[0] == "0") { // Quit all channels
		_clients[client_index].quitAllChannels();
		return ;
	}
	for (size_t i = 0; i < channels.size(); ++i) {
		Channel	chan(channels[i]);
		_clients[client_index].addChannel(chan);
	}
}

void	IrcServer::handleCommand(Command cmd, int client_index)
{
	t_command command_tag = commandNameToTag(cmd.getCommandName());
	std::vector<std::string> arguments = cmd.getArguments();
	std::string	response;
	
	switch (command_tag) {
		case PASS: {
			if (arguments[0] == _password) {
				_clients[client_index].authenticated = true;
			}
		} break;
		case NICK: {
			_clients[client_index].setNickName(arguments[0]);
		} break;
		case USER: {
			_clients[client_index].setUserName(arguments[0]);
		} break;
		case QUIT: {
			std::string quit_message = "Bye";
			if (arguments.size()) quit_message = arguments[0];
			std::string nick = _clients[client_index].getNickName();
			std::string user = _clients[client_index].getUserName();
			response = ":" + nick + "!" + user + "@localhost QUIT :" + quit_message + "\r\n";
			sendMessage(_clients[client_index], response);
			disconnectClient(_clients[client_index].getFd());
			return ;
		} break;
		case CAP: {
			if (arguments[0] == "END") return ;
			if (arguments[0] == "LS")
				response = ":"SERVER_NAME" CAP * LS :multi-prefix sasl\r\n";
			else if (arguments[0] == "REQ")
				response = ":"SERVER_NAME" CAP * ACK :multi-prefix\r\n";
			sendMessage(_clients[client_index], response);
		} break;
		case PRIVMSG: {
			std::string nick_to_send = arguments[0];
			std::string message_to_send = arguments[1];

			IrcLog::debug("MESSAGE: %s", message_to_send.c_str());

			size_t corresponding_client_index = getCorrespondingClient(nick_to_send);
			std::string response = ":" + _clients[client_index].getNickName()
			+ "!" + _clients[client_index].getUserName()
			+ "@" + _clients[client_index].getAddress()
			+ " PRIVMSG " + nick_to_send
			+ " :" + message_to_send + "\r\n";
			sendMessage(_clients[corresponding_client_index], response);
		} break;
		case JOIN: {
			handleJoinCommand(cmd, client_index);
		} break;
		case MODE: {
			// TODO: Try to understand this command
		} break;
		case UNKNOWN: {

		} break;
	}

	if (!_clients[client_index].registered) {
		tryToRegister(client_index);
	}
}

std::string	IrcServer::constructErrorResponse(std::string status, int client_index, std::string command_name, std::string message, size_t with_cmd_name)
{
	std::string	response = ":"SERVER_NAME" " + status + " " +
							(_clients[client_index].registered ? _clients[client_index].getNickName() : "*") +
						 	(with_cmd_name ? (" " + command_name) : "") +
							 " :" + message + "\r\n";
	return (response);
}

// TODO: refactor this function
void	IrcServer::parseCommand(std::string line, int client_index)
{
	Command cmd;
	std::string	response;

	std::string status = ParseCommand::parseCmd(line, cmd, _password, _clients, client_index);
	if (status == EMPTY_COMMAND) return ; // Ignore
	else if (status == ERR_NEEDMOREPARAMS) {
		response = constructErrorResponse(ERR_NEEDMOREPARAMS, client_index,
				cmd.getCommandName(), "Not enough parameters", WITH_COMMAND_NAME);
		sendMessage(_clients[client_index], response);
	} else if (status == ERR_PASSDMISMATCH) {
		response = constructErrorResponse(ERR_PASSDMISMATCH, client_index,
				cmd.getCommandName(), "Password incorrect", WITHOUT_COMMAND_NAME);
		sendMessage(_clients[client_index], response);
		disconnectClient(_clients[client_index].getFd());
	} else if (status == ERR_ALREADYREGISTERED) {
		response = constructErrorResponse(ERR_ALREADYREGISTERED, client_index,
				cmd.getCommandName(), "You may not reregister", WITHOUT_COMMAND_NAME);
		sendMessage(_clients[client_index], response);
	} else if (status == ERR_NONICKNAMEGIVEN) {
		response = constructErrorResponse(ERR_NONICKNAMEGIVEN, client_index,
				cmd.getCommandName(), "No nickname given", WITHOUT_COMMAND_NAME);
		sendMessage(_clients[client_index], response);
	} else if (status == ERR_ERRONEUSSNICKNAME) {
		response = constructErrorResponse(ERR_ERRONEUSSNICKNAME, client_index,
				cmd.getCommandName(), "Erroneous nickname", WITHOUT_COMMAND_NAME);
		sendMessage(_clients[client_index], response);
	} else if (status == ERR_NICKNAMEINUSE) {
		response = ":"SERVER_NAME" " + status + " " + cmd.getArguments()[0] +
					" :Nickname is already in use\r\n";
		sendMessage(_clients[client_index], response);
	} else if (status == ERR_NORECIPIENT) {
		response = ":"SERVER_NAME" " + status + " " + _clients[client_index].getNickName() +
					" :No recipient	given " + cmd.getCommandName() + "\r\n";
		sendMessage(_clients[client_index], response);
	} else if (status == ERR_NOTEXTTOSEND) {
		response = ":"SERVER_NAME" " + status + " " + _clients[client_index].getNickName() +
					" :No text to send\r\n";
		sendMessage(_clients[client_index], response);
	} else if (status == ERR_NOSUCHNICK) {
		response = ":"SERVER_NAME" " + status + " " + _clients[client_index].getNickName() +
					" " + cmd.getArguments()[0] + " :No such nick\r\n"; // or channel
		sendMessage(_clients[client_index], response);
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
		std::cout << "----------------------------------------------" << std::endl;
		std::cout << "Client " << fd << ", Data: " << message << std::endl;
		std::cout << "----------------------------------------------" << std::endl;
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
