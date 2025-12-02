#ifndef __MACRO_H__
# define __MACRO_H__

# include <cstring>
# include <cerrno>
# include "IrcException.hpp"

# define IRC_EXCEPTION(message) throw IrcException(message, __FILE__, __LINE__)

#endif // __MACRO_H__
