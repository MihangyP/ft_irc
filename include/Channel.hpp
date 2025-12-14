#ifndef __CHANNEL_HPP__
# define __CHANNEL_HPP__

# include <iostream>

class	Channel
{
	private:
		std::string	_name;
		//std::string _

	public:
		Channel(std::string chan_name);
		~Channel(void);

		std::string	getName(void) const;
};

#endif // __CHANNEL_HPP__
