#ifndef __CHANNEL_HPP__
# define __CHANNEL_HPP__

# include <iostream>
# include "IrcClient.hpp"

class	Channel
{
	private:
		std::string				_name;
		std::string 			_key;
		std::vector<IrcClient>	_members;
		std::vector<IrcClient>	_operators;
		std::string				_topic;

	public:
		Channel(std::string chan_name);
		Channel(const Channel& other);
		Channel& operator=(const Channel& other);
		~Channel(void);

		bool	isMember(IrcClient client) const;
		bool	isOperator(IrcClient op) const;

		void	addMember(IrcClient client);
		void	addOperator(IrcClient op);

		void	removeMember(IrcClient client);
		void	removeOperator(IrcClient op);


		std::string				getName(void) const;
		std::string 			getKey(void) const;
		std::vector<IrcClient>	getMembers(void) const;
		std::vector<IrcClient>	getOperators(void) const;

		void	setName(std::string name);
		void	setKey(std::string key);
};

#endif // __CHANNEL_HPP__
