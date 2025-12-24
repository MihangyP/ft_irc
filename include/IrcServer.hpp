#ifndef __IRC_SERVER_HPP__
# define __IRC_SERVER_HPP__

# include <iostream>
# include <vector>
# include <exception>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <poll.h>
# include <unistd.h>
# include <fcntl.h>
# include "IrcClient.hpp"
# include "Command.hpp"
# include "StringHelper.hpp"
# include "IrcLog.hpp"
# include "ParseCommand.hpp"
# include "constants.h"
# include "Channel.hpp"


// TODO: search what is the appropriate BACKLOG value
// TODO: make Client just a struct (class suckssss)
# define BACKLOG 69

class	IrcServer {
	private:
		static bool					signal;
		unsigned long				_port;
		std::string 				_password;
		int							_server_fd;
		std::vector<IrcClient>		_clients;
		std::vector<struct pollfd>	_fds;
		std::vector<Channel>		_available_channels;

	public:
		IrcServer(unsigned long port, std::string password);
		~IrcServer(void);

		static void				signalHandler(int signum);
		void					init(void);
		void					createSocket(void);
		void					addClient(void);
		void					readData(int fd);
		void					closeFds(void);
		void					parseReceivedData(std::string message, int fd);
		void					disconnectClient(int fd);
		void					parseCommand(std::string line, int client_index);
		void					tryToRegister(int client_index);
		int 					getCorrespondingClient(std::string nickname);
		std::vector<Channel>	getAvailableChannels(void) const;
		void					addIntoAvailableChannels(Channel chanel);
		int						alreadyAvailable(std::string name);
		std::string 			construct_name_list(std::string chan_name);
		int 					getChanIndex(const std::string chan_name);
		// TODO: put this in a HandleCommand class
		void		handleCommand(Command cmd, int client_index, t_channel_data& chan_data);
		void		handleJoinCommand(Command cmd, int client_index);
		void		handlePartCommand(Command cmd, int client_index);
		void		handleKickCommand(Command cmd, int client_index);
		void		handleInviteCommand(Command cmd, int client_index);
		void		handleModeCommand(Command cmd, int client_index);
		void		handleTopicCommand(Command cmd, int client_index, t_channel_data& chan_data);
		void		handleNamesCommand(Command cmd, int client_index);
		std::string	constructErrorResponse(std::string status, int client_index, std::string command_name, std::string message, size_t with_cmd_name);
};

#endif // __IRC_SERVER_HPP__
