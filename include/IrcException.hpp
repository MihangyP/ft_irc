#ifndef __IRC_EXCEPTION_HPP__
# define __IRC_EXCEPTION_HPP__

# include <exception>
# include <sstream>
# include "StringHelper.hpp"

class	IrcException: public std::exception {
	private:
		std::string	_message;
	public:
		IrcException(void);
		IrcException(std::string message, std::string file, int line);
		~IrcException(void) throw();

		const char *what(void) const throw();
};

#endif // __IRC_EXCEPTION_HPP__
