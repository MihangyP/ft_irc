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
		std::vector<IrcClient>	_invited;
		std::string				_topic;
		bool					_has_user_limit;
		size_t					_user_limit;
		bool					_invite_only;
		bool					_topic_restricted;

	public:
		Channel(std::string chan_name);
		Channel(const Channel& other);
		Channel& operator=(const Channel& other);
		~Channel(void);

		bool	isMember(IrcClient client) const;
		bool	isOperator(IrcClient op) const;
		bool	isInvited(IrcClient client) const;

		void	addMember(IrcClient client);
		void	addOperator(IrcClient op);
		void	addInvited(IrcClient client);

		void	removeMember(IrcClient client);
		void	removeOperator(IrcClient op);
		void	removeInvited(IrcClient client);

		bool	hasUserLimit(void) const;
		bool	isInviteOnly(void) const;
		bool	isTopicRestricted(void) const;


		std::string				getName(void) const;
		std::string 			getKey(void) const;
		std::string				getTopic(void) const;
		std::vector<IrcClient>	getMembers(void) const;
		std::vector<IrcClient>	getOperators(void) const;
		size_t					getUserLimit(void) const;

		void	setName(std::string name);
		void	setKey(std::string key);
		void	setTopic(std::string topic);
		void	setUserLimit(size_t user_limit);
		void	setInviteOnly(bool invite_only);
		void	setTopicRestricted(bool topic_restricted);
};

#endif // __CHANNEL_HPP__
