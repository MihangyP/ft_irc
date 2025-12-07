#include "IrcLog.hpp"

IrcLog::IrcLog(void)
{

}

IrcLog::~IrcLog(void)
{

}

void	IrcLog::error(std::string fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	std::cerr << "[ERROR]: ";
	size_t i = 0;
	while (i < fmt.size()) {
		if (fmt[i] == '%' && fmt[i + 1] ==  'i') {
			i += 2;
			int value = va_arg(args, int);
			std::cerr << value;
			continue ;
		}
		if (fmt[i] == '%' && fmt[i + 1] ==  's') {
			i += 2;
			const char *value = va_arg(args, const char *);
			std::cerr << value;
			continue ;
		}

		std::cerr << fmt[i];
		++i;
	}
	va_end(args);
	std::cerr << std::endl;
}

void	IrcLog::info(std::string fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	std::cout << "[INFO]: ";
	size_t i = 0;
	while (i < fmt.size()) {
		if (fmt[i] == '%' && fmt[i + 1] ==  'i') {
			i += 2;
			int value = va_arg(args, int);
			std::cout << value;
			continue ;
		}
		if (fmt[i] == '%' && fmt[i + 1] ==  's') {
			i += 2;
			const char *value = va_arg(args, const char *);
			std::cout << value;
			continue ;
		}

		std::cout << fmt[i];
		++i;
	}
	va_end(args);
	std::cout << std::endl;
}

void	IrcLog::debug(std::string fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	std::cout << "[DEBUG]: ";
	size_t i = 0;
	while (i < fmt.size()) {
		if (fmt[i] == '%' && fmt[i + 1] ==  'i') {
			i += 2;
			int value = va_arg(args, int);
			std::cout << value;
			continue ;
		}
		if (fmt[i] == '%' && fmt[i + 1] ==  's') {
			i += 2;
			const char *value = va_arg(args, const char *);
			std::cout << value;
			continue ;
		}

		std::cout << fmt[i];
		++i;
	}
	va_end(args);
	std::cout << std::endl;
}

void	IrcLog::warn(std::string fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	std::cout << "[WARNING]: ";
	size_t i = 0;
	while (i < fmt.size()) {
		if (fmt[i] == '%' && fmt[i + 1] ==  'i') {
			i += 2;
			int value = va_arg(args, int);
			std::cout << value;
			continue ;
		}
		if (fmt[i] == '%' && fmt[i + 1] ==  's') {
			i += 2;
			const char *value = va_arg(args, const char *);
			std::cout << value;
			continue ;
		}

		std::cout << fmt[i];
		++i;
	}
	va_end(args);
	std::cout << std::endl;
}
