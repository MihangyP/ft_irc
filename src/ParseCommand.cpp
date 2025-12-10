#include "ParseCommand.hpp"

ParseCommand::ParseCommand(void)
{

}

ParseCommand::~ParseCommand(void)
{

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
	if (command_name == "PASS") {
		if (arguments[0] == password) {
			client.authenticated = true;
		} else {
			std::string response = ":ft_irc 464 " + client.getNickName() + " :Password incorrect!\r\n";
			sendMessage(client, response);
			return (INVALID_PASSWORD);
		}
	} else if (command_name == "NICK") {
		client.setNickName(arguments[0]);
	} else if (command_name == "USER") {
		client.setUserName(arguments[0]);
	} else {
		IrcLog::info("Unkown command");
		return (UNKNOWN_COMMAND);
	}
	return (SUCCESS);
}
