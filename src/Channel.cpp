#include "Channel.hpp"
#include "ft_irc.h"

Channel::Channel(std::string chan_name)
{
	_name = chan_name;
	_key = "";
	_topic = "";
	_has_user_limit = false;
	_user_limit = 0;
	_invite_only = false;
	_topic_restricted = true;
}

Channel::Channel(const Channel& other)
{
	if (this != &other) {
		_name = other._name;	
		_key = other._key;	
		_members = other._members;	
		_operators = other._operators;
		_invited = other._invited;
		_topic = other._topic;
		_has_user_limit = other._has_user_limit;
		_user_limit = other._user_limit;
		_invite_only = other._invite_only;
		_topic_restricted = other._topic_restricted;
	}
}

Channel& Channel::operator=(const Channel& other)
{
	if (this != &other) {
		_name = other._name;	
		_key = other._key;	
		_members = other._members;	
		_operators = other._operators;
		_invited = other._invited;
		_topic = other._topic;
		_has_user_limit = other._has_user_limit;
		_user_limit = other._user_limit;
		_invite_only = other._invite_only;
		_topic_restricted = other._topic_restricted;
	}
	return (*this);
}

Channel::~Channel(void)
{
	// Nothing to put here
}

std::string	Channel::getName(void) const
{
	return (_name);
}

std::string	Channel::getKey(void) const
{
	return (_key);
}

std::vector<IrcClient> Channel::getMembers(void) const
{
	return (_members);
}

std::vector<IrcClient> Channel::getOperators(void) const
{
	return (_operators);
}

std::string	Channel::getTopic(void) const
{
	return (_topic);
}

size_t	Channel::getUserLimit(void) const
{
	return (_user_limit);
}

bool	Channel::hasUserLimit(void) const
{
	return (_has_user_limit);
}

void	Channel::setName(std::string name)
{
	_name = name;
}

void	Channel::setKey(std::string key)
{
	_key = key;
}

void	Channel::setTopic(std::string topic)
{
	_topic = topic;
}

void	Channel::setUserLimit(size_t user_limit)
{
	_user_limit = user_limit;
	if (user_limit > 0)
		_has_user_limit = true;
	else
		_has_user_limit = false;
}

bool	Channel::isMember(IrcClient client) const
{
	for (size_t i = 0; i < _members.size(); ++i) {
		if (irc_iequal(_members[i].getNickName(), client.getNickName()))
			return (true);
	}
	return (false);
}

bool	Channel::isOperator(IrcClient op) const
{
	for (size_t i = 0; i < _operators.size(); ++i) {
		if (irc_iequal(_operators[i].getNickName(), op.getNickName()))
			return (true);
	}
	return (false);
}

void	Channel::addMember(IrcClient client)
{
	_members.push_back(client);
}

void	Channel::addOperator(IrcClient op)
{
	_operators.push_back(op);
}

void	Channel::removeMember(IrcClient client)
{
	int pos = -1;
	for (size_t i = 0; i < _members.size(); ++i) {
		if (irc_iequal(_members[i].getNickName(), client.getNickName())) {
			pos = i;
			break ;
		}
	}
	if (pos != -1)
		_members.erase(_members.begin() + pos);
}

void	Channel::removeOperator(IrcClient op)
{
	int pos = -1;
	for (size_t i = 0; i < _operators.size(); ++i) {
		if (irc_iequal(_operators[i].getNickName(), op.getNickName())) {
			pos = i;
			break ;
		}
	}
	if (pos != -1)
		_operators.erase(_operators.begin() + pos);
}

bool	Channel::isInvited(IrcClient client) const
{
	for (size_t i = 0; i < _invited.size(); ++i) {
		if (irc_iequal(_invited[i].getNickName(), client.getNickName()))
			return (true);
	}
	return (false);
}

void	Channel::addInvited(IrcClient client)
{
	_invited.push_back(client);
}

void	Channel::removeInvited(IrcClient client)
{
	int pos = -1;
	for (size_t i = 0; i < _invited.size(); ++i) {
		if (irc_iequal(_invited[i].getNickName(), client.getNickName())) {
			pos = i;
			break ;
		}
	}
	if (pos != -1)
		_invited.erase(_invited.begin() + pos);
}

bool	Channel::isInviteOnly(void) const
{
	return (_invite_only);
}

bool	Channel::isTopicRestricted(void) const
{
	return (_topic_restricted);
}

void	Channel::setInviteOnly(bool invite_only)
{
	_invite_only = invite_only;
}

void	Channel::setTopicRestricted(bool topic_restricted)
{
	_topic_restricted = topic_restricted;
}
