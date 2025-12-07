#ifndef __IRC_LOG_HPP__
# define __IRC_LOG_HPP__

# include <iostream>
# include <cstdarg>

class IrcLog
{
	private:
		IrcLog(void);
		~IrcLog(void);

	public:
		static void	error(std::string fmt, ...);
		static void	info(std::string fmt, ...);
		static void	debug(std::string fmt, ...);
		static void	warn(std::string fmt, ...);
};

#endif // __IRC_LOG_HPP__
