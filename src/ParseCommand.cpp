#include "ParseCommand.hpp"

ParseCommand::ParseCommand(void)
{

}

ParseCommand::~ParseCommand(void)
{

}

std::string	checkCommandError(t_command cmd_tag, std::vector<std::string> arguments)
{
	switch (cmd_tag) {
		case PASS: {
			if (arguments.size() != 1) {
				return (ERR_NEEDMOREPARAMS);
			}
		} break;
		case NICK: {
			if (arguments.size() != 1) {
				return (ERR_NEEDMOREPARAMS);
			}
			if (arguments[0].size() > 9) { // nickname must be <= 9
				return (ERR_ERRONEUSSNICKNAME);
			}
		} break;
		case USER: {
			if (arguments.size() != 1) {
				return (ERR_NEEDMOREPARAMS);
			}
		} break;
		case UNKNOWN: {
			return (ERR_UNKNOWNCOMMAND);
		} break;
	}
	return (SUCCESS);
}
 
std::string	ParseCommand::parseCmd(const std::string& line, Command& cmd)
{
	Command	tmp_cmd;

	StringHelper	tmp(line);
	std::vector<std::string> tokens = tmp.trim().splitBySpace();
	if (tokens.empty())
		return (EMPTY_COMMAND);
	tmp_cmd.setCommandName(tokens[0]);	
	if (tokens.size() > 1) {
		tmp_cmd.setArguments(tokens.begin() + 1, tokens.end());
	}
	cmd = tmp_cmd;

	std::string	command_name = tmp_cmd.getCommandName();
	std::vector<std::string> arguments = tmp_cmd.getArguments();
	//// TODO: check errors for each command
	t_command command_tag = getAppropriateTag(command_name);
	std::string error = checkCommandError(command_tag, arguments);
	if (error != SUCCESS) {
		return (error);		
	}
	return (SUCCESS);
}
