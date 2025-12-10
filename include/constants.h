#ifndef __CONSTANTS_H__
# define __CONSTANTS_H__

# include <cstring>
# include <cerrno>
# include "IrcException.hpp"

typedef enum e_command {
	PASS,
	NICK,
	USER,
	UNKNOWN
} t_command;

// constant_values
# define SERVER_NAME "ft_irc"
# define MAX_MESSAGE_SIZE 1024
# define EMPTY_COMMAND ""
# define SUCCESS "SUCCESS"

// numeric replies
// - when registration succeeds
# define RPL_WELCOME 			"001"
# define RPL_YOURHOST 			"002"
# define RPL_CREATED 			"003"
# define RPL_MYINFO 			"004"
// - authentification / registration errors
# define ERR_PASSDMISMATCH		"464"
# define ERR_UNKNOWNCOMMAND 	"421"
# define ERR_NONICKNAMEGIVEN	"431"
# define ERR_ERRONEUSSNICKNAME	"432"
# define ERR_NICKNAMEINUSE		"433"
# define ERR_NEEDMOREPARAMS 	"461"
// - JOIN / PART / channel errors
# define ERR_NOTREGISTERED		"451"
# define ERR_NOSUCHCHANNEL		"403"
# define ERR_CANNOTSENDTOCHAN	"404"
# define ERR_CHANNELISFULL		"471"
# define ERR_INVITEONLYCHAN		"473"
# define ERR_BADCHANNELKEY		"475"
# define ERR_USERNOTINCHANNEL	"441"
# define ERR_NOTONCHANNEL		"442"
# define ERR_CHANOPRIVSNEEDED	"482"
// - INVITE / TOPIC / MODE successes
# define RPL_TOPIC				"332"
# define RPL_NOTOPIC			"331"
# define RPL_TOPICWHOTIME		"333"
# define RPL_INVITING			"341"
# define RPL_NAMREPLY			"353"
# define RPL_ENDOFNAMES			"366"

// macro
# define IRC_EXCEPTION(message) throw IrcException(message, __FILE__, __LINE__)

#endif // __CONSTANTS_H__
