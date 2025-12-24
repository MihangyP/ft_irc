#include "ParseCommand.hpp"
#include "ft_irc.h"

ParseCommand::ParseCommand(void)
{

}

ParseCommand::~ParseCommand(void)
{

}

bool	nicknameInUse(std::string nickname, std::vector<IrcClient> clients, int exclude_index)
{
	for (size_t i = 0; i < clients.size(); ++i) {
		if (exclude_index >= 0 && (int)i == exclude_index)
			continue;
		if (!clients[i].getNickName().empty() && irc_iequal(clients[i].getNickName(), nickname))
			return (true);
	}
	return (false);
}

bool	noSuchNick(std::string nickname, std::vector<IrcClient> clients)
{
	for (size_t i = 0; i < clients.size(); ++i) {
		if (clients[i].registered && irc_iequal(clients[i].getNickName(), nickname))
			return (false);
	}
	return (true);
}

int noSuchChannel(std::string name, std::vector<Channel> &channels)
{
	for (size_t i = 0; i < channels.size(); ++i) {
		if (irc_iequal(channels[i].getName(), name))
			return (i);
	}
	return (-1);
}

bool	channelHasNoTopic(std::vector<Channel>& channels, const std::string chan_name)
{
	for (size_t i = 0; i < channels.size(); ++i) {
		if (irc_iequal(channels[i].getName(), chan_name)) {
			return (channels[i].getTopic() == "");
		}
	}
	return (true);
}

bool	notOnChannel(std::vector<IrcClient>& clients, int client_index, std::vector<Channel>& chans, const std::string& chan_name)
{
	std::string nick = clients[client_index].getNickName();
	
	int chan_index = -1;
	for (size_t i = 0; i < chans.size(); ++i) {
		if (irc_iequal(chans[i].getName(), chan_name)) {
			chan_index = i;
			break;
		}
	}
	
	if (chan_index == -1)
		return (true);
	
	std::vector<IrcClient> members = chans[chan_index].getMembers();
	for (size_t i = 0; i < members.size(); ++i) {
		if (irc_iequal(members[i].getNickName(), nick))
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
			if (arguments[0].empty()) return (ERR_NONICKNAMEGIVEN);
			if (arguments[0].size() > 9) return (ERR_ERRONEUSSNICKNAME);
			for (size_t i = 0; i < arguments[0].size(); ++i) {
				char c = arguments[0][i];
				if (!isalnum(c) && c != '_' && c != '-' && c != '[' && c != ']' && 
					c != '{' && c != '}' && c != '\\' && c != '|') {
					return (ERR_ERRONEUSSNICKNAME);
				}
			}
			if (nicknameInUse(arguments[0], clients, client_index)) return (ERR_NICKNAMEINUSE);
		} break;
		case USER: {
			// USER <username> <hostname> <servername> <realname>
			if (arguments.size() < 4) return (ERR_NEEDMOREPARAMS);
			if (clients[client_index].registered) return(ERR_ALREADYREGISTERED);
			if (!clients[client_index].getUserName().empty()) return(ERR_ALREADYREGISTERED);
		} break;
		case QUIT: {
			 // Nothing to do	
		} break;
		case CAP: {
			if (!arguments.size()) return (ERR_NEEDMOREPARAMS);
		} break;
		case PRIVMSG: {
			if (!clients[client_index].registered) return (ERR_NOTREGISTERED);
			if (!arguments.size()) return (ERR_NORECIPIENT);
			if (arguments.size() == 1) return (ERR_NOTEXTTOSEND);
			
			// RFC 2812: PRIVMSG can have multiple targets separated by comma
			// We validate that at least one target exists, but we don't reject
			// if one of multiple targets is invalid (server will skip invalid ones)
			std::string targets_str = arguments[0];
			
			// Split by comma and check if at least one valid target exists
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
			
			// Validate at least first target exists
			if (targets.size() > 0) {
				std::string first_target = targets[0];
				if (!first_target.empty() && first_target[0] == '#') {
					int no_such_channel = noSuchChannel(first_target, channels);
					if (no_such_channel != -1) {
						chan_data.is_channel = true;
						chan_data.index = no_such_channel;
					}
				}
			}
		} break;
		case JOIN: {
			if (!clients[client_index].registered) return (ERR_NOTREGISTERED);
			if (!arguments.size()) return (ERR_NEEDMOREPARAMS);
		} break;
		case PART: {
			if (!clients[client_index].registered) return (ERR_NOTREGISTERED);
			if (!arguments.size()) return (ERR_NEEDMOREPARAMS);
			std::string chan_name = arguments[0];
			if (chan_name.empty() || chan_name[0] != '#') return (ERR_NOSUCHCHANNEL);
			if (notOnChannel(clients, client_index, channels, chan_name)) return (ERR_NOTONCHANNEL);
		} break;
		case KICK: {
			if (!clients[client_index].registered) return (ERR_NOTREGISTERED);
			if (arguments.size() < 2) return (ERR_NEEDMOREPARAMS);
			std::string chan_name = arguments[0];
			if (chan_name.empty() || chan_name[0] != '#') return (ERR_NOSUCHCHANNEL);
			if (notOnChannel(clients, client_index, channels, chan_name)) return (ERR_NOTONCHANNEL);
		} break;
		case INVITE: {
			if (!clients[client_index].registered) return (ERR_NOTREGISTERED);
			if (arguments.size() < 2) return (ERR_NEEDMOREPARAMS);
			std::string chan_name = arguments[1];
			if (chan_name.empty() || chan_name[0] != '#') return (ERR_NOSUCHCHANNEL);
			if (notOnChannel(clients, client_index, channels, chan_name)) return (ERR_NOTONCHANNEL);
		} break;
		case MODE: {
			if (!clients[client_index].registered) return (ERR_NOTREGISTERED);
			if (!arguments.size()) return (ERR_NEEDMOREPARAMS);
			std::string target = arguments[0];
			if (!target.empty() && target[0] == '#') {
				if (notOnChannel(clients, client_index, channels, target)) return (ERR_NOTONCHANNEL);
			}
		} break;
		case TOPIC: {
			if (!clients[client_index].registered) return (ERR_NOTREGISTERED);
			if (!arguments.size()) return (ERR_NEEDMOREPARAMS);
			std::string chan_name = arguments[0];
			if (chan_name.empty() || chan_name[0] != '#') return (ERR_NOSUCHCHANNEL);
			if (notOnChannel(clients, client_index, channels, chan_name)) return (ERR_NOTONCHANNEL);
			if (arguments.size() == 1 && channelHasNoTopic(channels, chan_name)) return (RPL_NOTOPIC);
		} break;
		case NAMES: {
			if (!clients[client_index].registered) return (ERR_NOTREGISTERED);
			if (arguments.size() > 0) {
				std::string chan_name = arguments[0];
				if (!chan_name.empty() && chan_name[0] == '#') {
					if (notOnChannel(clients, client_index, channels, chan_name)) return (ERR_NOTONCHANNEL);
				}
			}
		} break;
		case PING: {

		} break;
		case PONG: {

		} break;
		case UNKNOWN: {
			return (ERR_UNKNOWNCOMMAND);
		} break;
	}
	return (SUCCESS);
}

std::string	ParseCommand::parseCmd(const std::string& line, Command& cmd, std::string password,
	std::vector<IrcClient> clients, int client_index, std::vector<Channel>& channels, t_channel_data& chan_data)
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
