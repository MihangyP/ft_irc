#ifndef __COMMAND_HPP__
# define __COMMAND_HPP__

# include <string>
# include <vector>

// NOTE: I don't want to write this entire stuff everytime I will use it
// Just use itr as an alias
typedef std::vector<std::string>::const_iterator itr;

class	Command {
	private:
		std::string					_command_name;
		std::vector<std::string>	_arguments;

	public:
		Command(void);
		Command(const Command &other);
		Command& operator=(const Command &other);
		~Command(void);

		void	setCommandName(std::string command_name);
		void	setArguments(itr begin, itr end);

		std::string					getCommandName(void) const;
		std::vector<std::string>	getArguments(void) const;
};

#endif // __COMMAND_HPP__
