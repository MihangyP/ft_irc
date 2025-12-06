#ifndef __IRC_CLIENT_HPP__
# define __IRC_CLIENT_HPP__

# include <iostream>
# include <unistd.h>
# include "Command.hpp"

class	IrcClient {
	private:
		int						_fd;
		std::string				_address;
		bool					_is_connected;
		bool					_authenticated;
		std::vector<Command>	_auth_buf;

	public:
		IrcClient(int fd, std::string address);
		~IrcClient(void);

		int						getFd(void) const;
		std::string				getAddress(void) const;
		bool					isConnected(void) const;
		bool					isAuthenticated(void) const;
		std::vector<Command>	getAuthBuf(void) const;

		void		setIsConnected(bool status);
		void		setAuthenticated(bool status);
		void		pushIntoAuthBuf(Command cmd);
		void		clearAuthBuf(void);
};

#endif // __IRC_SERVER_HPP__
