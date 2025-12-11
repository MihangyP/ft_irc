#ifndef __FT_IRC_H__
# define __FT_IRC_H__

# include <iostream>
# include <cstdlib>
# include <csignal>
# include "constants.h"
# include "IrcClient.hpp"

void		sendMessage(const IrcClient& client, const std::string& message);
t_command	commandNameToTag(const std::string& command_name);

#endif // __FT_IRC_HPP__
