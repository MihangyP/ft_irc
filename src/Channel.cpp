#include "Channel.hpp"

Channel::Channel(std::string chan_name)
{
	_name = chan_name;
	_key = "";
	_topic = "";
}

Channel::Channel(const Channel& other)
{
	if (this != &other) {
		_name = other._name;	
		_key = other._key;	
		_members = other._members;	
		_operators = other._operators;
		_topic = other._topic;
	}
}

Channel& Channel::operator=(const Channel& other)
{
	if (this != &other) {
		_name = other._name;	
		_key = other._key;	
		_members = other._members;	
		_operators = other._operators;
		_topic = other._topic;
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

void	Channel::setName(std::string name)
{
	_name = name;
}

void	Channel::setKey(std::string key)
{
	_key = key;
}

bool	Channel::isMember(IrcClient client) const
{
	for (size_t i = 0; i < _members.size(); ++i) {
		if (_members[i].getNickName() == client.getNickName())
			return (true);
	}
	return (false);
}

bool	Channel::isOperator(IrcClient op) const
{
	for (size_t i = 0; i < _operators.size(); ++i) {
		if (_operators[i].getNickName() == op.getNickName())
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
		if (_members[i].getNickName() == client.getNickName()) {
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
		if (_operators[i].getNickName() == op.getNickName()) {
			pos = i;
			break ;
		}
	}
	if (pos != -1)
		_operators.erase(_operators.begin() + pos);
}
