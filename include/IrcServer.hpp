#ifndef __IRC_SERVER_HPP__
# define __IRC_SERVER_HPP__

# include <iostream>
# include <vector>
# include <exception>
# include <sys/socket.h>
# include <netinet/in.h>
# include <poll.h>
# include "IrcClient.hpp"
# include "macro.h"

// TODO: search what is the appropriate BACKLOG value
# define BACKLOG 69

class	IrcServer {
	private:
		size_t						_port;
		std::string 				_password;
		int							_server_fd;
		std::vector<IrcClient>		_clients;
		std::vector<struct pollfd>	_fds;

	public:
		IrcServer(size_t port, std::string password);
		IrcServer(const IrcServer& other);
		IrcServer& operator=(const IrcServer &other);
		~IrcServer(void);

		void	init(void);
		void	createSocket(void);
};

#endif // __IRC_SERVER_HPP__
