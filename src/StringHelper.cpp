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

// TODO: manage only spaces (IT SEGGGG)
StringHelper 				StringHelper::trim(void)
{
	int	start = 0;
	int	end = _content.size() - 1;
	int	i = 0;
	while (std::isspace(_content[i])) {
		++start;
		++i;
	}
	i = end;
	while (std::isspace(_content[i])) {
		--end;
		--i;
	}
	_content = _content.substr(start, end - start + 1);
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
