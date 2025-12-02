#include "IrcClient.hpp"

IrcClient::IrcClient()
{
	std::cout << "IrcClient Default Constructor Called!!" << std::endl;
}

IrcClient::IrcClient(const IrcClient& other)
{
	(void)other;
	std::cout << "IrcClient Copy Constructor Called!!" << std::endl;
}

IrcClient& IrcClient::operator=(const IrcClient &other)
{
	(void)other;
	std::cout << "IrcClient Assignment Operator Called!!" << std::endl;
	return (*this);
}

IrcClient::~IrcClient(void)
{
	std::cout << "IrcClient Destructor Called!!" << std::endl;
}
