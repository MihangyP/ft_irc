#include "IrcServer.hpp"
#include "ft_irc.h"

bool IrcServer::signal = false;

void	IrcServer::signalHandler(int signum)
{
	std::cout << std::endl;
	IrcLog::info("receive signal %i", signum);
	IrcServer::signal = true;
}

IrcServer::IrcServer(unsigned long port, std::string password)
{
	_server_fd = -1;
	_port = port;
	_password = password;
	IrcLog::info("=========== FT_IRC =========== ");
	IrcLog::info("port = %i", _port);
	IrcLog::info("password = %s", _password.c_str());
	IrcLog::info("============================== ");

}

IrcServer::~IrcServer(void)
{
	closeFds();
}

std::vector<Channel>	IrcServer::getAvailableChannels(void) const
{
	return (_available_channels);
}

void	IrcServer::addIntoAvailableChannels(Channel channel)
{
	_available_channels.push_back(channel);
}

void	IrcServer::addClient(void)
{
	struct sockaddr_in	addr;
	struct pollfd		poll;
	int					client_fd;
	socklen_t			addr_size = sizeof addr;

	client_fd = accept(_server_fd, (struct sockaddr *)&addr, &addr_size);
	if (client_fd == -1)
		IRC_EXCEPTION("Accept failed");
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
	send(client.getFd(), message.c_str(), message.size(), MSG_NOSIGNAL);
}

void	IrcServer::disconnectClient(int fd)
{
	int client_index = -1;
	
	for (size_t i = 0; i < _clients.size(); ++i) {
		if (_clients[i].getFd() == fd) {
			client_index = i;
			break;
		}
	}
	
	if (client_index != -1) {
		for (size_t i = 0; i < _available_channels.size(); ) {
			if (_available_channels[i].isMember(_clients[client_index])) {
				_available_channels[i].removeMember(_clients[client_index]);
				_available_channels[i].removeOperator(_clients[client_index]);
				
				if (_available_channels[i].getMembers().empty()) {
					IrcLog::info("Channel %s deleted (no members left after disconnect)", 
								_available_channels[i].getName().c_str());
					_available_channels.erase(_available_channels.begin() + i);
					continue;
				}
			}
			++i;
		}
	}
	
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
	else if (command_name == "PART") return (PART);
	else if (command_name == "KICK") return (KICK);
	else if (command_name == "INVITE") return (INVITE);
	else if (command_name == "MODE") return (MODE);
	else if (command_name == "TOPIC") return (TOPIC);
	else if (command_name == "NAMES") return (NAMES);
	else if (command_name == "PING") return (PING);
	else if (command_name == "PONG") return (PONG);
	else return (UNKNOWN);
}

int 	IrcServer::getCorrespondingClient(std::string nickname)
{
	for (size_t i = 0; i < _clients.size(); ++i) {
		if (irc_iequal(_clients[i].getNickName(), nickname)) {
			return (int)i;
		}
	}
	return -1;
}

int	IrcServer::alreadyAvailable(std::string name)
{
	for (size_t i = 0; i < _available_channels.size(); ++i) {
		if (irc_iequal(_available_channels[i].getName(), name))
			return (i);
	}
	return (-1);	
}

std::string IrcServer::construct_name_list(std::string chan_name)
{
	int chan_index = -1;
	for (int i = 0; i < (int)_available_channels.size(); ++i) {
		if (irc_iequal(_available_channels[i].getName(), chan_name)) {
			chan_index = i;
			break;
		}
	}
	
	if (chan_index == -1)
		return ("");
	
	std::string result;
	std::string operators_names;
	std::string members_names;

	std::vector<IrcClient> members = _available_channels[chan_index].getMembers();
	std::vector<IrcClient> operators = _available_channels[chan_index].getOperators();
	
	for (size_t i = 0; i < operators.size(); ++i) {
		if (i > 0)
			operators_names.append(" ");
		operators_names.append("@" + operators[i].getNickName());
	}
	for (size_t i = 0; i < members.size(); ++i) {
		if (!_available_channels[chan_index].isOperator(members[i])) {
			if (!members_names.empty())
				members_names.append(" ");
			members_names.append(members[i].getNickName());
		}
	}
	
	if (!operators_names.empty() && !members_names.empty())
		result = operators_names + " " + members_names;
	else if (!operators_names.empty())
		result = operators_names;
	else
		result = members_names;
		
	return (result);
}

void	IrcServer::handleJoinCommand(Command cmd, int client_index)
{
	std::vector<std::string> arguments = cmd.getArguments();
	std::string nick = _clients[client_index].getNickName();
	std::string user = _clients[client_index].getUserName();
	std::string response;

	StringHelper channels_sh;
	StringHelper keys_sh;
	if (arguments.size())
		channels_sh.setContent(arguments[0]);
	if (arguments.size() >= 2)
		keys_sh.setContent(arguments[1]);
	std::vector<std::string>	channels = channels_sh.trim().splitByDelimiter(',');
	std::vector<std::string>	keys = keys_sh.trim().splitByDelimiter(',');
	if (channels.size() == 1 && channels[0] == "0") {
		for (size_t i = 0; i < _available_channels.size(); ++i) {
			if (_available_channels[i].isMember(_clients[client_index])) {
				_available_channels[i].removeMember(_clients[client_index]);
			}
		}
		return ;
	}

	for (size_t i = 0; i < channels.size(); ++i) {
		if (channels[i].empty() || channels[i][0] != '#') {
			response = ":" SERVER_NAME " " ERR_NOSUCHCHANNEL " " + nick +
					   " " + channels[i] + " :No such channel\r\n";
			sendMessage(_clients[client_index], response);
			continue;
		}
		
		Channel chan(channels[i]);
		int found = alreadyAvailable(chan.getName());
		if (found == -1) {
			addIntoAvailableChannels(chan);
			_available_channels[_available_channels.size() - 1].addMember(_clients[client_index]);
			_available_channels[_available_channels.size() - 1].addOperator(_clients[client_index]);
			// Confirmation
			{
				response = ":" + nick + "!" + user + "@localhost" + " JOIN " + channels[i] + "\r\n";
				sendMessage(_clients[client_index], response);
			}
			// Mode (automatic operator)
			{
				response = ":" SERVER_NAME " MODE " + channels[i] + " +o " + nick + "\r\n";
				sendMessage(_clients[client_index], response);
			}
			// Topic
			{
				response = ":" SERVER_NAME " 331 " + nick + " " + channels[i] + " :No topic is set\r\n";
				sendMessage(_clients[client_index], response);
			}
			// NAMES
			// :server 353 nick = #test :@nick
			{
				response = ":" SERVER_NAME " 353 " + nick + " = " +  channels[i] + " :@" + nick + "\r\n";
				sendMessage(_clients[client_index], response);
			}
			// End of NAMES
			{
				response = ":" SERVER_NAME " 366 " + nick + " " + channels[i] + " :End of /NAMES list\r\n";
				sendMessage(_clients[client_index], response);
			}
		} else { // join a channel
			bool is_empty_channel = _available_channels[found].getMembers().empty();
			
			if (_available_channels[found].getKey() != "") {
				std::string provided_key = (i < keys.size()) ? keys[i] : "";
				if (_available_channels[found].getKey() != provided_key) {
					response = ":" SERVER_NAME " " ERR_BADCHANNELKEY " " + nick +
							   " " + channels[i] + " :Cannot join channel (+k)\r\n";
					sendMessage(_clients[client_index], response);
					continue;
				}
			}

			if (_available_channels[found].isInviteOnly() &&
				!_available_channels[found].isInvited(_clients[client_index])) {
				response = ":" SERVER_NAME " " ERR_INVITEONLYCHAN " " + nick +
						   " " + channels[i] + " :Cannot join channel (+i)\r\n";
				sendMessage(_clients[client_index], response);
				continue;
			}

			if (_available_channels[found].hasUserLimit()) {
				size_t current_size = _available_channels[found].getMembers().size();
				size_t limit = _available_channels[found].getUserLimit();
				if (current_size >= limit) {
					response = ":" SERVER_NAME " " ERR_CHANNELISFULL " " + nick +
							   " " + channels[i] + " :Cannot join channel (+l)\r\n";
					sendMessage(_clients[client_index], response);
					IrcLog::info("User %s rejected from %s (channel full)", nick.c_str(), channels[i].c_str());
					continue;
				}
			}

			{
				const std::vector<IrcClient>& members = _available_channels[found].getMembers();
				for (size_t j = 0; j < members.size(); ++j) {
					int c_index = getCorrespondingClient(members[j].getNickName());
					response = ":" + nick + "!" + user + "@localhost" + " JOIN " + channels[i] + "\r\n";
					sendMessage(_clients[c_index], response);
				}
			}

			_available_channels[found].addMember(_clients[client_index]);
			
			if (is_empty_channel) {
				_available_channels[found].addOperator(_clients[client_index]);
				IrcLog::info("User %s became operator of recreated channel %s", nick.c_str(), channels[i].c_str());
			}
			
			_available_channels[found].removeInvited(_clients[client_index]);

			{
				response = ":" + nick + "!" + user + "@localhost" + " JOIN " + channels[i] + "\r\n";
				sendMessage(_clients[client_index], response);
			}
			{
				std::string topic = _available_channels[found].getTopic();
				if (topic != "") {
					response = ":" SERVER_NAME " " RPL_TOPIC " " + nick + " " + channels[i] + " :" + topic + "\r\n";
				} else {
					response = ":" SERVER_NAME " 331 " + nick + " " + channels[i] + " :No topic is set\r\n";
				}
				sendMessage(_clients[client_index], response);
			}
			// NAMES
			{
				//:server 353 nick = #channel :@op1 +voice1 user2 user3
				std::string names_list = construct_name_list(channels[i]);
				response = ":" SERVER_NAME " 353 " + nick + " = " + channels[i] + " :" + names_list + "\r\n";
				sendMessage(_clients[client_index], response);
			}
			// End of NAMES
			{
				response = ":" SERVER_NAME " 366 " + nick + " " + channels[i] + " :End of /NAMES list\r\n";
				sendMessage(_clients[client_index], response);
			}
		}
	}

}

int IrcServer::getChanIndex(const std::string chan_name)
{
	for (int i = 0; i < (int)_available_channels.size(); ++i) {
		if (_available_channels[i].getName() == chan_name)
			return (i);
	}
	return (-1);
}

void	IrcServer::handleCommand(Command cmd, int client_index, t_channel_data& /* chan_data */)
{
	t_command command_tag = commandNameToTag(cmd.getCommandName());
	std::vector<std::string> arguments = cmd.getArguments();
	std::string nick = _clients[client_index].getNickName();
	std::string user = _clients[client_index].getUserName();
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
			
			std::string response;
			if (!nick.empty() && !user.empty()) {
				response = ":" + nick + "!" + user + "@localhost QUIT :" + quit_message + "\r\n";
			}
			
			if (!response.empty()) {
				std::vector<int> notified_clients;
				for (size_t i = 0; i < _available_channels.size(); ++i) {
					if (_available_channels[i].isMember(_clients[client_index])) {
						const std::vector<IrcClient>& members = _available_channels[i].getMembers();
						for (size_t j = 0; j < members.size(); ++j) {
							int c_index = getCorrespondingClient(members[j].getNickName());
							if (c_index >= 0 && c_index != client_index) {
								bool already_notified = false;
								for (size_t k = 0; k < notified_clients.size(); ++k) {
									if (notified_clients[k] == c_index) {
										already_notified = true;
										break;
									}
								}
								if (!already_notified) {
									sendMessage(_clients[c_index], response);
									notified_clients.push_back(c_index);
								}
							}
						}
					}
				}
			}
			
			std::string error_msg = "ERROR :Closing Link: localhost (Client Quit)\r\n";
			sendMessage(_clients[client_index], error_msg);
			
			disconnectClient(_clients[client_index].getFd());
			return ;
		} break;
		case CAP: {
			if (arguments[0] == "END") return ;
			if (arguments[0] == "LS")
				response = ":" SERVER_NAME " CAP * LS :multi-prefix sasl\r\n";
			else if (arguments[0] == "REQ")
				response = ":" SERVER_NAME " CAP * ACK :multi-prefix\r\n";
			sendMessage(_clients[client_index], response);
		} break;
		case PRIVMSG: {
			const std::string& targets_str = arguments[0];
			const std::string& text = arguments[1];

			// Split targets by comma (RFC 2812: multiple targets separated by comma)
			std::vector<std::string> targets;
			std::string current_target;
			for (size_t i = 0; i < targets_str.size(); ++i) {
				if (targets_str[i] == ',') {
					if (!current_target.empty()) {
						targets.push_back(current_target);
						current_target.clear();
					}
				} else {
					current_target += targets_str[i];
				}
			}
			if (!current_target.empty()) {
				targets.push_back(current_target);
			}

			// Send message to each target
			for (size_t t = 0; t < targets.size(); ++t) {
				const std::string& target = targets[t];
				
				// Build response for this specific target
				std::string response = ":" + _clients[client_index].getNickName()
					  + "!" + user
					  + "@" + _clients[client_index].getAddress()
					  + " PRIVMSG " + target
					  + " :" + text + "\r\n";

				// Check if target is a channel
				if (!target.empty() && target[0] == '#') {
					int chan_index = -1;
					for (size_t i = 0; i < _available_channels.size(); ++i) {
						if (irc_iequal(_available_channels[i].getName(), target)) {
							chan_index = i;
							break;
						}
					}
					
					if (chan_index >= 0) {
						// Send to all members except sender
						const std::vector<IrcClient>& members = _available_channels[chan_index].getMembers();
						for (size_t i = 0; i < members.size(); ++i) {
							int c_index = getCorrespondingClient(members[i].getNickName());
							if (c_index >= 0 && c_index != client_index)
								sendMessage(_clients[c_index], response);
						}
					}
				} else {
					// Target is a user
					int target_index = getCorrespondingClient(target);
					if (target_index >= 0) {
						sendMessage(_clients[target_index], response);
					}
				}
			}
		} break;
		case JOIN: {
			handleJoinCommand(cmd, client_index);
		} break;
		case PART: {
			handlePartCommand(cmd, client_index);
		} break;
		case KICK: {
			handleKickCommand(cmd, client_index);
		} break;
		case INVITE: {
			handleInviteCommand(cmd, client_index);
		} break;
		case MODE: {
			handleModeCommand(cmd, client_index);
		} break;
		case TOPIC: {
			handleTopicCommand(cmd, client_index);
		} break;
		case NAMES: {
			handleNamesCommand(cmd, client_index);
		} break;
		case PING: {
			std::string token = arguments.size() > 0 ? arguments[0] : SERVER_NAME;
			response = ":" SERVER_NAME " PONG " SERVER_NAME " :" + token + "\r\n";
			sendMessage(_clients[client_index], response);
		} break;
		case PONG: {
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
	std::string	response = ":" SERVER_NAME " " + status + " " +
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
	t_channel_data channel_data;
	channel_data.is_channel = false;
	channel_data.index = -1;
	std::string nick = _clients[client_index].getNickName();

	std::string status = ParseCommand::parseCmd(line, cmd, _password, _clients, client_index, _available_channels, channel_data);
	if (status == EMPTY_COMMAND) return ; // Ignore
	else if (status == ERR_NEEDMOREPARAMS) {
		response = constructErrorResponse(ERR_NEEDMOREPARAMS, client_index,
				cmd.getCommandName(), "Not enough parameters", WITH_COMMAND_NAME);
		sendMessage(_clients[client_index], response);
	} else if (status == ERR_PASSDMISMATCH) {
		response = constructErrorResponse(ERR_PASSDMISMATCH, client_index,
				cmd.getCommandName(), "Password incorrect", WITHOUT_COMMAND_NAME);
		sendMessage(_clients[client_index], response);
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
	} else if (status == ERR_NOTREGISTERED) {
		response = constructErrorResponse(ERR_NOTREGISTERED, client_index,
				cmd.getCommandName(), "You have not registered", WITHOUT_COMMAND_NAME);
		sendMessage(_clients[client_index], response);
	} else if (status == ERR_NICKNAMEINUSE) {
		std::string current_nick = nick.empty() ? "*" : nick;
		response = ":" SERVER_NAME " " + status + " " + current_nick + " " + 
					cmd.getArguments()[0] + " :Nickname is already in use\r\n";
		sendMessage(_clients[client_index], response);
	} else if (status == ERR_NORECIPIENT) {
		response = ":" SERVER_NAME " " + status + " " + nick +
					" :No recipient	given " + cmd.getCommandName() + "\r\n";
		sendMessage(_clients[client_index], response);
	} else if (status == ERR_NOTEXTTOSEND) {
		response = ":" SERVER_NAME " " + status + " " + nick +
					" :No text to send\r\n";
		sendMessage(_clients[client_index], response);
	} else if (status == ERR_NOSUCHNICK) {
		response = ":" SERVER_NAME " " + status + " " + nick +
					" " + cmd.getArguments()[0] + " :No such nick\r\n";
		sendMessage(_clients[client_index], response);
	} else if (status == RPL_NOTOPIC) {
		response = ":" SERVER_NAME " " + status + " " + nick +
			      " " + cmd.getArguments()[0] + " :No topic is set\r\n";
		sendMessage(_clients[client_index], response);
	} else if (status == ERR_NOTONCHANNEL)  {
		response = ":" SERVER_NAME " " + status + " " + nick + " " + cmd.getArguments()[0] + 
					" :You're not on that channel\r\n";
		sendMessage(_clients[client_index], response);
	} else if (status == SUCCESS) {
		handleCommand(cmd, client_index, channel_data);
	}
}

void	IrcServer::readData(int fd)
{
	char	message[MAX_MESSAGE_SIZE];
	for (size_t i = 0; i < MAX_MESSAGE_SIZE; ++i)
		message[i] = 0;

	ssize_t	bytes_read = recv(fd, message, sizeof message - 1, 0);
	if (bytes_read > 0) {
		message[bytes_read] = '\0';
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
			
			if (line.size() > 510) {
				IrcLog::info("Message from client %d exceeds 512 chars, truncating", fd);
				line = line.substr(0, 510);
			}
			
			parseCommand(line, i);
			pos = _clients[i].input_buffer.find("\n");
		}
		
		if (_clients[i].input_buffer.size() > 512) {
			IrcLog::info("Input buffer from client %i exceeds 512 chars without newline, clearing", fd);
			_clients[i].input_buffer.clear();
		}
	} else {
		disconnectClient(fd);
	}
}

void	IrcServer::init(void)
{
	createSocket();
	IrcLog::info("IrcServer [%i] connected", _server_fd);

	while (!IrcServer::signal) {
		int	status;
		status = poll(&_fds[0], _fds.size(), -1);
		if (status == -1 && !IrcServer::signal)
			IRC_EXCEPTION("Poll failed");
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
		IRC_EXCEPTION("Socket creation failed");
	int opt = 1;
	status = setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);
	if (status == -1)
		IRC_EXCEPTION("Setsockopt failed");
	status = bind(_server_fd, (struct sockaddr *)&addr, sizeof addr);
	if (status == -1)
		IRC_EXCEPTION("Bind failed");
	status = listen(_server_fd, BACKLOG);
	if (status == -1)
		IRC_EXCEPTION("Listen failed");

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

void	IrcServer::handlePartCommand(Command cmd, int client_index)
{
	std::vector<std::string> arguments = cmd.getArguments();
	std::string nick = _clients[client_index].getNickName();
	std::string user = _clients[client_index].getUserName();
	std::string part_msg = "Leaving";
	
	if (arguments.size() >= 2)
		part_msg = arguments[1];

	StringHelper channels_sh(arguments[0]);
	std::vector<std::string> channels = channels_sh.trim().splitByDelimiter(',');

	for (size_t i = 0; i < channels.size(); ++i) {
		if (channels[i].empty() || channels[i][0] != '#')
			continue;
			
		int chan_index = alreadyAvailable(channels[i]);
		if (chan_index != -1) {
			std::string response = ":" + nick + "!" + user + "@localhost PART " + 
								   channels[i] + " :" + part_msg + "\r\n";
			const std::vector<IrcClient>& members = _available_channels[chan_index].getMembers();
			for (size_t j = 0; j < members.size(); ++j) {
				int c_index = getCorrespondingClient(members[j].getNickName());
				sendMessage(_clients[c_index], response);
			}
			
			_available_channels[chan_index].removeMember(_clients[client_index]);
			_available_channels[chan_index].removeOperator(_clients[client_index]);
			
			if (_available_channels[chan_index].getMembers().empty()) {
				IrcLog::info("Channel %s deleted (no members left)", channels[i].c_str());
				_available_channels.erase(_available_channels.begin() + chan_index);
			}
		}
	}
}

void	IrcServer::handleKickCommand(Command cmd, int client_index)
{
	std::vector<std::string> arguments = cmd.getArguments();
	std::string nick = _clients[client_index].getNickName();
	std::string user = _clients[client_index].getUserName();
	std::string chan_name = arguments[0];
	std::string target_nick = arguments[1];
	std::string kick_msg = "Kicked";
	
	if (chan_name.empty() || chan_name[0] != '#') {
		std::string response = ":" SERVER_NAME " " ERR_NOSUCHCHANNEL " " + nick +
							   " " + chan_name + " :No such channel\r\n";
		sendMessage(_clients[client_index], response);
		return;
	}
	
	if (arguments.size() >= 3)
		kick_msg = arguments[2];

	int chan_index = alreadyAvailable(chan_name);
	if (chan_index == -1) {
		std::string response = ":" SERVER_NAME " " ERR_NOSUCHCHANNEL " " + nick +
							   " " + chan_name + " :No such channel\r\n";
		sendMessage(_clients[client_index], response);
		return;
	}

	if (!_available_channels[chan_index].isOperator(_clients[client_index])) {
		std::string response = ":" SERVER_NAME " " ERR_CHANOPRIVSNEEDED " " + nick +
							   " " + chan_name + " :You're not channel operator\r\n";
		sendMessage(_clients[client_index], response);
		return;
	}

	int target_index = getCorrespondingClient(target_nick);
	
	if (irc_iequal(target_nick, nick)) {
		std::string response = ":" SERVER_NAME " " ERR_USERNOTINCHANNEL " " + nick +
							   " " + target_nick + " " + chan_name + " :You cannot kick yourself\r\n";
		sendMessage(_clients[client_index], response);
		return;
	}

	if (!_available_channels[chan_index].isMember(_clients[target_index])) {
		std::string response = ":" SERVER_NAME " " ERR_USERNOTINCHANNEL " " + nick +
							   " " + target_nick + " " + chan_name + " :They aren't on that channel\r\n";
		sendMessage(_clients[client_index], response);
		return;
	}

	std::string response = ":" + nick + "!" + user + "@localhost KICK " + 
						   chan_name + " " + target_nick + " :" + kick_msg + "\r\n";
	const std::vector<IrcClient>& members = _available_channels[chan_index].getMembers();
	for (size_t i = 0; i < members.size(); ++i) {
		int c_index = getCorrespondingClient(members[i].getNickName());
		sendMessage(_clients[c_index], response);
	}

	_available_channels[chan_index].removeMember(_clients[target_index]);
	_available_channels[chan_index].removeOperator(_clients[target_index]);

	if (_available_channels[chan_index].getMembers().empty()) {
		IrcLog::info("Channel %s deleted (no members left after kick)", chan_name.c_str());
		_available_channels.erase(_available_channels.begin() + chan_index);
	}
}

void	IrcServer::handleInviteCommand(Command cmd, int client_index)
{
	std::vector<std::string> arguments = cmd.getArguments();
	std::string nick = _clients[client_index].getNickName();
	std::string user = _clients[client_index].getUserName();
	std::string target_nick = arguments[0];
	std::string chan_name = arguments[1];

	if (chan_name.empty() || chan_name[0] != '#') {
		std::string response = ":" SERVER_NAME " " ERR_NOSUCHCHANNEL " " + nick +
							   " " + chan_name + " :No such channel\r\n";
		sendMessage(_clients[client_index], response);
		return;
	}

	int chan_index = alreadyAvailable(chan_name);
	if (chan_index == -1) {
		std::string response = ":" SERVER_NAME " " ERR_NOSUCHCHANNEL " " + nick +
							   " " + chan_name + " :No such channel\r\n";
		sendMessage(_clients[client_index], response);
		return;
	}

	if (!_available_channels[chan_index].isMember(_clients[client_index])) {
		std::string response = ":" SERVER_NAME " " ERR_NOTONCHANNEL " " + nick +
							   " " + chan_name + " :You're not on that channel\r\n";
		sendMessage(_clients[client_index], response);
		return;
	}

	if (_available_channels[chan_index].isInviteOnly() && 
		!_available_channels[chan_index].isOperator(_clients[client_index])) {
		std::string response = ":" SERVER_NAME " " ERR_CHANOPRIVSNEEDED " " + nick +
							   " " + chan_name + " :You're not channel operator\r\n";
		sendMessage(_clients[client_index], response);
		return;
	}

	int target_index = getCorrespondingClient(target_nick);
	if (target_index == 0 && _clients[0].getNickName() != target_nick) {
		std::string response = ":" SERVER_NAME " " ERR_NOSUCHNICK " " + nick +
							   " " + target_nick + " :No such nick\r\n";
		sendMessage(_clients[client_index], response);
		return;
	}

	if (_available_channels[chan_index].isMember(_clients[target_index])) {
		std::string response = ":" SERVER_NAME " " ERR_USERONCHANNEL " " + nick +
							   " " + target_nick + " " + chan_name + " :is already on channel\r\n";
		sendMessage(_clients[client_index], response);
		return;
	}

	_available_channels[chan_index].addInvited(_clients[target_index]);

	std::string response = ":" SERVER_NAME " " RPL_INVITING " " + nick +
						   " " + target_nick + " " + chan_name + "\r\n";
	sendMessage(_clients[client_index], response);

	response = ":" + nick + "!" + user + "@localhost INVITE " + 
			   target_nick + " :" + chan_name + "\r\n";
	sendMessage(_clients[target_index], response);
}

void	IrcServer::handleModeCommand(Command cmd, int client_index)
{
	std::vector<std::string> arguments = cmd.getArguments();
	std::string nick = _clients[client_index].getNickName();
	std::string target = arguments[0];

	if (!target.empty() && target[0] == '#') {
		int chan_index = alreadyAvailable(target);
		if (chan_index == -1) {
			std::string response = ":" SERVER_NAME " " ERR_NOSUCHCHANNEL " " + nick +
								   " " + target + " :No such channel\r\n";
			sendMessage(_clients[client_index], response);
			return;
		}

		if (arguments.size() == 1) {
			return;
		}

		if (!_available_channels[chan_index].isOperator(_clients[client_index])) {
			std::string response = ":" SERVER_NAME " " ERR_CHANOPRIVSNEEDED " " + nick +
								   " " + target + " :You're not channel operator\r\n";
			sendMessage(_clients[client_index], response);
			return;
		}

		std::string mode_str = arguments[1];
		bool adding = true;
		size_t arg_index = 2;

		for (size_t i = 0; i < mode_str.size(); ++i) {
			char mode = mode_str[i];
			if (mode == '+') {
				adding = true;
			} else if (mode == '-') {
				adding = false;
			} else if (mode == 'i') {
				_available_channels[chan_index].setInviteOnly(adding);
				std::string response = ":" + nick + "!" + _clients[client_index].getUserName() +
									   "@localhost MODE " + target + " " + 
									   (adding ? "+i" : "-i") + "\r\n";
				const std::vector<IrcClient>& members = _available_channels[chan_index].getMembers();
				for (size_t j = 0; j < members.size(); ++j) {
					int c_index = getCorrespondingClient(members[j].getNickName());
					sendMessage(_clients[c_index], response);
				}
			} else if (mode == 't') {
				_available_channels[chan_index].setTopicRestricted(adding);
				std::string response = ":" + nick + "!" + _clients[client_index].getUserName() +
									   "@localhost MODE " + target + " " + 
									   (adding ? "+t" : "-t") + "\r\n";
				const std::vector<IrcClient>& members = _available_channels[chan_index].getMembers();
				for (size_t j = 0; j < members.size(); ++j) {
					int c_index = getCorrespondingClient(members[j].getNickName());
					sendMessage(_clients[c_index], response);
				}
			} else if (mode == 'k') {
				if (arg_index < arguments.size()) {
					if (adding) {
						_available_channels[chan_index].setKey(arguments[arg_index]);
					} else {
						_available_channels[chan_index].setKey("");
					}
					std::string response = ":" + nick + "!" + _clients[client_index].getUserName() +
										   "@localhost MODE " + target + " " + 
										   (adding ? "+k " : "-k ") + 
										   (adding ? arguments[arg_index] : "") + "\r\n";
					const std::vector<IrcClient>& members = _available_channels[chan_index].getMembers();
					for (size_t j = 0; j < members.size(); ++j) {
						int c_index = getCorrespondingClient(members[j].getNickName());
						sendMessage(_clients[c_index], response);
					}
					arg_index++;
				}
			} else if (mode == 'o') {
				if (arg_index < arguments.size()) {
					std::string target_nick = arguments[arg_index];
					int target_index = -1;
					for (size_t j = 0; j < _clients.size(); ++j) {
						if (irc_iequal(_clients[j].getNickName(), target_nick)) {
							target_index = j;
							break;
						}
					}
					if (target_index != -1 && _available_channels[chan_index].isMember(_clients[target_index])) {
						if (!adding && irc_iequal(target_nick, nick)) {
							std::string response = ":" SERVER_NAME " " ERR_CHANOPRIVSNEEDED " " + nick +
												   " " + target + " :You cannot remove your own operator status\r\n";
							sendMessage(_clients[client_index], response);
							arg_index++;
							continue;
						}
						
						if (adding) {
							if (!_available_channels[chan_index].isOperator(_clients[target_index])) {
								_available_channels[chan_index].addOperator(_clients[target_index]);
							}
						} else {
							if (_available_channels[chan_index].isOperator(_clients[target_index])) {
								_available_channels[chan_index].removeOperator(_clients[target_index]);
							}
						}
						std::string response = ":" + nick + "!" + _clients[client_index].getUserName() +
											   "@localhost MODE " + target + " " + 
											   (adding ? "+o " : "-o ") + target_nick + "\r\n";
						const std::vector<IrcClient>& members = _available_channels[chan_index].getMembers();
						for (size_t j = 0; j < members.size(); ++j) {
							int c_index = getCorrespondingClient(members[j].getNickName());
							sendMessage(_clients[c_index], response);
						}
					}
					arg_index++;
				}
			} else if (mode == 'l') {
				if (adding && arg_index < arguments.size()) {
					size_t limit = std::atoi(arguments[arg_index].c_str());
					_available_channels[chan_index].setUserLimit(limit);
					std::string response = ":" + nick + "!" + _clients[client_index].getUserName() +
										   "@localhost MODE " + target + " +l " + 
										   arguments[arg_index] + "\r\n";
					const std::vector<IrcClient>& members = _available_channels[chan_index].getMembers();
					for (size_t j = 0; j < members.size(); ++j) {
						int c_index = getCorrespondingClient(members[j].getNickName());
						sendMessage(_clients[c_index], response);
					}
					arg_index++;
				} else if (!adding) {
					_available_channels[chan_index].setUserLimit(0);
					std::string response = ":" + nick + "!" + _clients[client_index].getUserName() +
										   "@localhost MODE " + target + " -l\r\n";
					const std::vector<IrcClient>& members = _available_channels[chan_index].getMembers();
					for (size_t j = 0; j < members.size(); ++j) {
						int c_index = getCorrespondingClient(members[j].getNickName());
						sendMessage(_clients[c_index], response);
					}
				}
			}
		}
	}
}

void	IrcServer::handleTopicCommand(Command cmd, int client_index)
{
	std::vector<std::string> arguments = cmd.getArguments();
	std::string nick = _clients[client_index].getNickName();
	std::string user = _clients[client_index].getUserName();
	std::string chan_name = arguments[0];
	std::string response;

	int chan_index = getChanIndex(chan_name);
	
	if (chan_index == -1) {
		response = ":" SERVER_NAME " " ERR_NOSUCHCHANNEL " " + nick +
				   " " + chan_name + " :No such channel\r\n";
		sendMessage(_clients[client_index], response);
		return;
	}
	
	if (arguments.size() == 1) {
		std::string topic = _available_channels[chan_index].getTopic();
		if (topic.empty()) {
			response = ":" SERVER_NAME " " RPL_NOTOPIC " " + nick + " " + chan_name +
						" :No topic is set\r\n";
		} else {
			response = ":" SERVER_NAME " " RPL_TOPIC " " + nick + " " + chan_name +
						" :" + topic + "\r\n";
		}
		sendMessage(_clients[client_index], response);
	} else {
		if (_available_channels[chan_index].isTopicRestricted() &&
			!_available_channels[chan_index].isOperator(_clients[client_index])) {
			response = ":" SERVER_NAME " " ERR_CHANOPRIVSNEEDED " " + nick +
					   " " + chan_name + " :You're not channel operator\r\n";
			sendMessage(_clients[client_index], response);
			return;
		}

		std::string new_topic = arguments[1];
		_available_channels[chan_index].setTopic(new_topic);
		
		response = ":" + nick + "!" + user + "@localhost TOPIC " + chan_name + " :" + 
					new_topic + "\r\n";
		const std::vector<IrcClient>& members = _available_channels[chan_index].getMembers();
		for (size_t i = 0; i < members.size(); ++i) {
			int c_index = getCorrespondingClient(members[i].getNickName());
			sendMessage(_clients[c_index], response);
		}
	}
}

void	IrcServer::handleNamesCommand(Command cmd, int client_index)
{
	std::vector<std::string> arguments = cmd.getArguments();
	std::string nick = _clients[client_index].getNickName();
	std::string response;
	
	if (arguments.size() == 0) {
		for (size_t i = 0; i < _available_channels.size(); ++i) {
			std::string chan_name = _available_channels[i].getName();
			std::string names_list = construct_name_list(chan_name);
			
			response = ":" SERVER_NAME " " RPL_NAMREPLY " " + nick + " = " + 
					   chan_name + " :" + names_list + "\r\n";
			sendMessage(_clients[client_index], response);
			
			response = ":" SERVER_NAME " " RPL_ENDOFNAMES " " + nick + " " + 
					   chan_name + " :End of /NAMES list\r\n";
			sendMessage(_clients[client_index], response);
		}
	} else {
		for (size_t i = 0; i < arguments.size(); ++i) {
			std::string chan_name = arguments[i];
			int chan_index = getChanIndex(chan_name);
			
			if (chan_index == -1) {
				response = ":" SERVER_NAME " " RPL_ENDOFNAMES " " + nick + " " + 
						   chan_name + " :End of /NAMES list\r\n";
				sendMessage(_clients[client_index], response);
			} else {
				std::string names_list = construct_name_list(chan_name);
				
				response = ":" SERVER_NAME " " RPL_NAMREPLY " " + nick + " = " + 
						   chan_name + " :" + names_list + "\r\n";
				sendMessage(_clients[client_index], response);
				
				response = ":" SERVER_NAME " " RPL_ENDOFNAMES " " + nick + " " + 
						   chan_name + " :End of /NAMES list\r\n";
				sendMessage(_clients[client_index], response);
			}
		}
	}
}
