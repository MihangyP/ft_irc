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

std::string StringHelper::toString(int number)
{
	std::ostringstream	string_stream;

	string_stream << number;
	return (string_stream.str());
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

std::vector<std::string>	StringHelper::splitByDelimiter(char del)
{
	std::vector<std::string>	tokens;
	size_t pos = _content.find(del);
	size_t	start = 0;
	while (pos != std::string::npos) {
		tokens.push_back(_content.substr(start, pos - start));
		start = pos + 1;
		pos = _content.find(del, start);
	}
	tokens.push_back(_content.substr(start));
	return (tokens);
}
