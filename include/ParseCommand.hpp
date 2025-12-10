#ifndef __PARSE_COMMAND_HPP__
# define __PARSE_COMMAND_HPP__

# include <iostream>
# include "IrcClient.hpp"
# include "StringHelper.hpp"
# include "IrcLog.hpp"
# include "macro.h"

void	sendMessage(IrcClient client, const std::string& message);

class	ParseCommand
{
	private:
		ParseCommand(void);
		~ParseCommand(void);
	public:
		static t_status	parseCmd(IrcClient& client, const std::string& line, std::string password);
};

#endif // __PARSE_COMMAND_HPP__
