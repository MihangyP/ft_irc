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

std::string	checkCommandError(t_command cmd_tag, std::vector<std::string> arguments, std::string password, std::vector<IrcClient> clients, int client_index)
{
	switch (cmd_tag) {
		case PASS: {
			if (arguments.size() != 1) return (ERR_NEEDMOREPARAMS);
			if (clients[client_index].registered) return(ERR_ALREADYREGISTERED);
			if (arguments[0] != password) return (ERR_PASSDMISMATCH);
		} break;
		case NICK: {
			if (arguments.size() != 1) return (ERR_NONICKNAMEGIVEN);
			if (clients[client_index].registered) return(ERR_ALREADYREGISTERED);
			if (arguments[0].size() > 9) return (ERR_ERRONEUSSNICKNAME);
			if (nicknameInUse(arguments[0], clients)) return (ERR_NICKNAMEINUSE);
		} break;
		case USER: {
			// TODO: parse correctly (":")
			//if (arguments.size() != 4) return (ERR_NEEDMOREPARAMS);
			// TODO: check parameter errors
			if (clients[client_index].registered) return(ERR_ALREADYREGISTERED);
		} break;
		case QUIT: {
			 // Nothing to do	
		} break;
		case CAP: {
			if (!arguments.size()) return (ERR_NEEDMOREPARAMS);
			// TODO: manage other errors
		} break;
		case PRIVMSG: {
			if (!arguments.size()) return (ERR_NORECIPIENT);
			if (noSuchNick(arguments[0], clients)) return (ERR_NOSUCHNICK);
			if (arguments.size() == 1) return (ERR_NOTEXTTOSEND);
		} break;
		case JOIN: {

		} break;
		case UNKNOWN: {
			return (ERR_UNKNOWNCOMMAND);
		} break;
	}
	return (SUCCESS);
}

std::string	ParseCommand::parseCmd(const std::string& line, Command& cmd, std::string password, std::vector<IrcClient> clients, int client_index)
{
	Command	tmp_cmd;

	StringHelper	tmp(line);
	std::vector<std::string> tokens = tmp.trim().splitBySpace();
	if (tokens.empty())
		return (EMPTY_COMMAND);

	tmp_cmd.setCommandName(tokens[0]);

	// Build arguments but treat the first token that starts with ':' as the start of the trailing parameter
	std::vector<std::string> args;
	for (size_t i = 1; i < tokens.size(); ++i) {
		if (!tokens[i].empty() && tokens[i][0] == ':') {
			// trailing parameter: join the current token (without leading ':') and all remaining tokens with spaces
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
	std::string error = checkCommandError(command_tag, arguments, password, clients, client_index);
	if (error != SUCCESS) {
		return (error);		
	}
	return (SUCCESS);
}
