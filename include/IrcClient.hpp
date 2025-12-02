#ifndef __IRC_CLIENT_HPP__
# define __IRC_CLIENT_HPP__

# include <iostream>

class	IrcClient {
	private:

	public:
		IrcClient();
		IrcClient(const IrcClient& other);
		IrcClient& operator=(const IrcClient &other);
		~IrcClient(void);
};

#endif // __IRC_SERVER_HPP__
