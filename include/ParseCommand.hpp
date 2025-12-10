#ifndef __PARSE_COMMAND_HPP__
# define __PARSE_COMMAND_HPP__

# include <iostream>
# include "IrcClient.hpp"
# include "StringHelper.hpp"
# include "IrcLog.hpp"
# include "constants.h"
# include "ft_irc.h"

class	ParseCommand
{
	private:
		ParseCommand(void);
		~ParseCommand(void);
	public:
		static std::string	parseCmd(const std::string& line, Command& cmd);
};

#endif // __PARSE_COMMAND_HPP__
