#ifndef __IRC_CLIENT_HPP__
# define __IRC_CLIENT_HPP__

# include <iostream>
# include <unistd.h>
# include "Command.hpp"

class	IrcClient {
	private:
		int						_fd;
		std::string				_address;
		std::string				_nickname;
		std::string				_username;
		int						_mode;
		// TODO: what means _unused ?
		std::string				_unused;
		std::string				_realname;

	public:
		std::string				input_buffer;
		bool					authenticated;
		bool					registered;

		IrcClient(int fd, std::string address);
		~IrcClient(void);

		int						getFd(void) const;
		std::string				getAddress(void) const;
		std::string				getNickName(void) const;
		std::string				getUserName(void) const;

		void		setNickName(std::string nickname);
		void		setUserName(std::string username);
};

#endif // __IRC_SERVER_HPP__
