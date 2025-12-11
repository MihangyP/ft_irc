#ifndef __STRING_HELPER_HPP__
# define __STRING_HELPER_HPP__

# include <string>
# include <vector>
# include <sstream>
# include <cctype>

class	StringHelper
{
	private:
		std::string	_content;

	public:
		StringHelper(std::string content);
		StringHelper(const StringHelper &other);
		StringHelper& operator=(const StringHelper &other);
		~StringHelper(void);

		StringHelper 				trim(void);
		std::vector<std::string>	splitBySpace(void);		
		static std::string 			toString(int number);
};

#endif // __STRING_HELPER_HPP__
