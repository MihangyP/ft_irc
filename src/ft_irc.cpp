#include "ft_irc.h"
#include "IrcServer.hpp"
#include "IrcException.hpp"

void	usage(std::string program_name)
{
	IrcLog::error("Usage: %s <port> <password>", program_name.c_str());
	std::exit(1);
}

int	main(int ac, char **av)
{
	if (ac != 3) {
		usage(av[0]);
	}
	// TODO: check if valid port or valid password ?
	// valid port: 0 to 65535
	int 		port = std::atoi(av[1]);
	if (!(0 < port && port < 65535)) {
		IrcLog::error("Invalid port: %i", port);
		IrcLog::error("    A valid port must start from 0 to 65535" );
		return (1);
	}
	std::string password = av[2];
	IrcServer	server(port, password);
	try {
		signal(SIGINT, IrcServer::signalHandler);
		signal(SIGQUIT, IrcServer::signalHandler);
		server.init();
	} catch (std::exception &e) {
		IrcLog::error("%s", e.what());
		return (1);
	}
	return (0);
}
