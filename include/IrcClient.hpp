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
		std::string				_nickname;
		std::string				_username;

	public:
		std::string				input_buffer;

		IrcClient(int fd, std::string address);
		~IrcClient(void);

		int						getFd(void) const;
		std::string				getAddress(void) const;
		bool					isConnected(void) const;
		bool					isAuthenticated(void) const;
		std::vector<Command>	getAuthBuf(void) const;
		std::string				getNickName(void) const;
		std::string				getUserName(void) const;

		void		setNickName(std::string nickname);
		void		setUserName(std::string username);
		void		setIsConnected(bool status);
		void		setAuthenticated(bool status);
		void		pushIntoAuthBuf(Command cmd);
		void		clearAuthBuf(void);
};

#endif // __IRC_SERVER_HPP__
