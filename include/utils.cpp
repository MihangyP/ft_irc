#include "ft_irc.h"
#include "IrcClient.hpp"

void sendMessage(const IrcClient& client, const std::string& message)
{
	send(client.getFd(), message.c_str(), message.size(), 0);
}

