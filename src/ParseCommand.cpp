#include "ParseCommand.hpp"

ParseCommand::ParseCommand(void)
{

}

ParseCommand::~ParseCommand(void)
{

}

t_command	getAppropriateTag(const std::string& command_name)
{
	if (command_name == "PASS") return (PASS);
	else if (command_name == "NICK") return (NICK);
	else if (command_name == "USER") return (USER);
	else return (UNKNOWN);
}

bool	checkCommandError(t_command cmd_tag, std::vector<std::string> arguments)
{
	switch (cmd_tag) {
		case PASS: {
			if (arguments.size() != 1) return (true);
		} break;
		case NICK: {
			if (arguments.size() != 1) return (true);
			if (arguments[0].size() > 9) return (true);
		} break;
		case USER: {
			if (arguments.size() != 1) return (true);
		} break;
		case UNKNOWN: {
			return (true);
		} break;
	}
	return (false);
}
 
t_status	ParseCommand::parseCmd(IrcClient& client, const std::string& line, std::string password)
{
	Command	cmd;

	StringHelper	tmp(line);
	std::vector<std::string> tokens = tmp.trim().splitBySpace();
	if (tokens.empty())
		return (EMPTY_COMMAND);
	cmd.setCommandName(tokens[0]);	
	if (tokens.size() > 1) {
		cmd.setArguments(tokens.begin() + 1, tokens.end());
	}

	std::string	command_name = cmd.getCommandName();
	std::vector<std::string> arguments = cmd.getArguments();
	//// TODO: check errors for each command
	t_command command_tag = getAppropriateTag(command_name);
	if (checkCommandError(command_tag, arguments)) {
		return (ARGUMENTS_ERROR);	
	}
	if (command_tag == PASS) {
		if (arguments[0] == password) {
			client.authenticated = true;
		} else {
			std::string response = ":ft_irc 464 " + client.getNickName() + " :Password incorrect!\r\n";
			sendMessage(client, response);
			return (INVALID_PASSWORD);
		}
	} else if (command_tag == NICK) {
		client.setNickName(arguments[0]);
	} else if (command_tag == USER) {
		client.setUserName(arguments[0]);
	} else {
		IrcLog::info("Unkown command");
		return (UNKNOWN_COMMAND);
	}
	return (SUCCESS);
}
