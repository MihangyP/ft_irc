#ifndef __FT_IRC_H__
# define __FT_IRC_H__

# include <iostream>
# include <cstdlib>
# include <csignal>
# include "IrcClient.hpp"

void	sendMessage(IrcClient client, const std::string& message);

#endif // __FT_IRC_HPP__
