#ifndef __MACRO_H__
# define __MACRO_H__

# include <cstring>
# include <cerrno>
# include "IrcException.hpp"

typedef enum e_status {
	SUCCESS,
	INVALID_PASSWORD,
	EMPTY_COMMAND,
	UNKNOWN_COMMAND
} t_status;

typedef enum e_command {
	PASS,
	NICK,
	USER,
	UNKNOWN
} t_command;

# define MAX_MESSAGE_SIZE 1024

# define IRC_EXCEPTION(message) throw IrcException(message, __FILE__, __LINE__)

#endif // __MACRO_H__
