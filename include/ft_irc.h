#ifndef __FT_IRC_H__
# define __FT_IRC_H__

# include <iostream>
# include <cstdlib>
# include <csignal>
# include <string>
# include "constants.h"
# include "IrcClient.hpp"

void		sendMessage(const IrcClient& client, const std::string& message);
t_command	commandNameToTag(const std::string& command_name);

// RFC 2812: Case-insensitive comparison for IRC nicknames and channels
std::string	irc_tolower(const std::string& str);
bool		irc_iequal(const std::string& a, const std::string& b);

#endif // __FT_IRC_HPP__
