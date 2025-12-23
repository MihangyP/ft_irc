#include "ParseCommand.hpp"

ParseCommand::ParseCommand(void)
{

}

ParseCommand::~ParseCommand(void)
{

}

bool	nicknameInUse(std::string nickname, std::vector<IrcClient> clients)
{
	for (size_t i = 0; i < clients.size(); ++i) {
		if (clients[i].registered && clients[i].getNickName() == nickname)
			return (true);
	}
	return (false);
}

bool	noSuchNick(std::string nickname, std::vector<IrcClient> clients)
{
	for (size_t i = 0; i < clients.size(); ++i) {
		if (clients[i].registered && clients[i].getNickName() == nickname)
			return (false);
	}
	return (true);
}

int noSuchChannel(std::string name, std::vector<Channel> &channels)
{
	for (size_t i = 0; i < channels.size(); ++i) {
		if (channels[i].getName() == name)
			return (i);
	}
	return (-1);
}

bool	channelHasNoTopic(std::vector<Channel>& channels, const std::string chan_name)
{
	for (size_t i = 0; i < channels.size(); ++i) {
		if (channels[i].getName() == chan_name) {
			IrcLog::debug("chan_name: %s", channels[i].getName().c_str());
			return (channels[i].getTopic() == "");
		}
	}
	// Channel not found, assume it has no topic
	return (true);
}

bool	notOnChannel(std::vector<IrcClient>& clients, int client_index, std::vector<Channel>& chans, const std::string& chan_name)
{
	std::string nick = clients[client_index].getNickName();
	
	// Find the channel
	int chan_index = -1;
	for (size_t i = 0; i < chans.size(); ++i) {
		if (chans[i].getName() == chan_name) {
			chan_index = i;
			break;
		}
	}
	
	// If channel not found, user is definitely not on it
	if (chan_index == -1)
		return (true);
	
	// Check if user is in the channel
	std::vector<IrcClient> members = chans[chan_index].getMembers();
	for (size_t i = 0; i < members.size(); ++i) {
		if (members[i].getNickName() == nick)
			return (false);
	}
	return (true);
}

std::string	checkCommandError(t_command cmd_tag, std::vector<std::string> arguments, std::string password, std::vector<IrcClient> clients, int client_index, std::vector<Channel>& channels, t_channel_data& chan_data)
{
	switch (cmd_tag) {
		case PASS: {
			if (arguments.size() != 1) return (ERR_NEEDMOREPARAMS);
			if (clients[client_index].registered) return(ERR_ALREADYREGISTERED);
			if (clients[client_index].authenticated) return(ERR_ALREADYREGISTERED);
			if (arguments[0] != password) return (ERR_PASSDMISMATCH);
		} break;
		case NICK: {
			if (arguments.size() != 1) return (ERR_NONICKNAMEGIVEN);
			if (clients[client_index].registered) return(ERR_ALREADYREGISTERED);
			if (arguments[0].empty()) return (ERR_NONICKNAMEGIVEN);
			if (arguments[0].size() > 9) return (ERR_ERRONEUSSNICKNAME);
			// Check for invalid characters in nickname
			for (size_t i = 0; i < arguments[0].size(); ++i) {
				char c = arguments[0][i];
				if (!isalnum(c) && c != '_' && c != '-' && c != '[' && c != ']' && 
					c != '{' && c != '}' && c != '\\' && c != '|') {
					return (ERR_ERRONEUSSNICKNAME);
				}
			}
			if (nicknameInUse(arguments[0], clients)) return (ERR_NICKNAMEINUSE);
		} break;
		case USER: {
			// USER <username> <hostname> <servername> <realname>
			// realname can have spaces if prefixed with ':'
			if (arguments.size() < 4) return (ERR_NEEDMOREPARAMS);
			if (clients[client_index].registered) return(ERR_ALREADYREGISTERED);
			// Check if username is already set (prevent double USER command)
			if (!clients[client_index].getUserName().empty()) return(ERR_ALREADYREGISTERED);
		} break;
		case QUIT: {
			 // Nothing to do	
		} break;
		case CAP: {
			if (!arguments.size()) return (ERR_NEEDMOREPARAMS);
			// TODO: manage other errors
		} break;
		case PRIVMSG: {
			// TODO: manage error params (e.g. no ':' to enter a msg)
			if (!arguments.size()) return (ERR_NORECIPIENT);
			std::string name = arguments[0];
			if (!name.empty() && name[0] == '#') {
				int no_such_channel = noSuchChannel(name, channels);
				if (no_such_channel == -1) return (ERR_NOSUCHNICK);
				chan_data.is_channel = true;
				chan_data.index = no_such_channel;
			} else {
				if (noSuchNick(name, clients)) return (ERR_NOSUCHNICK);
			}
			if (arguments.size() == 1) return (ERR_NOTEXTTOSEND);
		} break;
		case JOIN: {
			if (!arguments.size()) return (ERR_NEEDMOREPARAMS);
			// TODO: handle more errors
		} break;
		case PART: {
			if (!arguments.size()) return (ERR_NEEDMOREPARAMS);
		} break;
		case KICK: {
			if (arguments.size() < 2) return (ERR_NEEDMOREPARAMS);
		} break;
		case INVITE: {
			if (arguments.size() < 2) return (ERR_NEEDMOREPARAMS);
		} break;
		case MODE: {
			if (!arguments.size()) return (ERR_NEEDMOREPARAMS);
		} break;
		case TOPIC: {
			if (!arguments.size()) return (ERR_NEEDMOREPARAMS);
			std::string chan_name = arguments[0];
			// Validate channel name format
			if (chan_name.empty() || chan_name[0] != '#') return (ERR_NOSUCHCHANNEL);
			if (notOnChannel(clients, client_index, channels, chan_name)) return (ERR_NOTONCHANNEL);
			if (arguments.size() == 1 && channelHasNoTopic(channels, chan_name)) return (RPL_NOTOPIC);
			// TODO: handle ERR_CHANOPRIVSNEEDED
		} break;
		case PING: {
			// PING is allowed even before registration
		} break;
		case PONG: {
			// PONG is allowed even before registration
		} break;
		case UNKNOWN: {
			return (ERR_UNKNOWNCOMMAND);
		} break;
	}
	return (SUCCESS);
}

std::string	ParseCommand::parseCmd(const std::string& line, Command& cmd, std::string password, std::vector<IrcClient> clients, int client_index, std::vector<Channel>& channels, t_channel_data& chan_data)
{
	Command	tmp_cmd;

	StringHelper	tmp(line);
	std::vector<std::string> tokens = tmp.trim().splitBySpace();
	if (tokens.empty())
		return (EMPTY_COMMAND);

	tmp_cmd.setCommandName(tokens[0]);

	std::vector<std::string> args;
	for (size_t i = 1; i < tokens.size(); ++i) {
		if (!tokens[i].empty() && tokens[i][0] == ':') {
			std::string trailing = tokens[i].substr(1);
			for (size_t j = i + 1; j < tokens.size(); ++j) {
				trailing += " " + tokens[j];
			}
			args.push_back(trailing);
			break;
		} else {
			args.push_back(tokens[i]);
		}
	}

	if (!args.empty()) {
		tmp_cmd.setArguments(args.begin(), args.end());
	}

	cmd = tmp_cmd;

	std::string	command_name = tmp_cmd.getCommandName();
	std::vector<std::string> arguments = tmp_cmd.getArguments();

	t_command command_tag = commandNameToTag(command_name);
	std::string error = checkCommandError(command_tag, arguments, password, clients, client_index, channels, chan_data);
	if (error != SUCCESS) {
		return (error);
	}
	return (SUCCESS);
}
