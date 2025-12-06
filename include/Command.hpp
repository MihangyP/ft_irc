#ifndef __COMMAND_HPP__
# define __COMMAND_HPP__

# include <string>
# include <vector>

class	Command {
	private:
		std::string					_command_name;
		std::vector<std::string>	_arguments;

	public:
		Command();
		Command(const Command &other);
		Command& operator=(const Command &other);
		~Command();

		void	setCommandName(std::string command_name);
		void	setArguments(std::vector<std::string> arguments);
};

#endif // __COMMAND_HPP__
