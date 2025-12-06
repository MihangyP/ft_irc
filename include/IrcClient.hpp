#ifndef __IRC_CLIENT_HPP__
# define __IRC_CLIENT_HPP__

# include <iostream>
# include <unistd.h>

class	IrcClient {
	private:
		int			_fd;
		std::string	_address;
		bool		_is_connected;

	public:
		IrcClient(int fd, std::string address);
		IrcClient(const IrcClient& other);
		IrcClient& operator=(const IrcClient &other);
		~IrcClient(void);

		int			getFd(void) const;
		std::string	getAddress(void) const;
		bool		isConnected(void) const;

		void		setIsConnected(bool status);
};

#endif // __IRC_SERVER_HPP__
