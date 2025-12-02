#ifndef __IRC_EXCEPTION_HPP__
# define __IRC_EXCEPTION_HPP__

# include <exception>

class	IrcException: public std::exception {
	private:
	
	public:
		IrcException();
		~IrcException();
};

#endif // __IRC_EXCEPTION_HPP__
