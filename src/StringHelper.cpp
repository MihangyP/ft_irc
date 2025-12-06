#include "StringHelper.hpp"

StringHelper::StringHelper(std::string content)
{
	_content = content;
}

StringHelper::StringHelper(const StringHelper &other)
{
	if (this != &other) {
		_content = other._content;
	}
}

StringHelper& StringHelper::operator=(const StringHelper &other)
{
	if (this != &other) {
		_content = other._content;
	}
	return (*this);
}

StringHelper::~StringHelper(void)
{

}

StringHelper 				StringHelper::trim(void)
{
	return (*this);
}

std::vector<std::string>	StringHelper::splitBySpace(void)
{
	std::vector<std::string>	tokens;
	std::stringstream			ss(_content);
	std::string					token;

	while (ss >> token) {
		tokens.push_back(token);
	}
	return (tokens);
}
