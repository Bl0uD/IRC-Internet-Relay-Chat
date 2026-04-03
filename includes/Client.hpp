#pragma once

#include "Include.hpp"

class Client
{
	private:
		/*int			_fd;
		std::string	_ipAddress;*/

	public:
		~Client();
		Client();
		Client( std::string );
		Client( const Client &copy );
		Client &operator=( const Client &instance );

		/*int			getFd( void ) const;
		std::string	getIpAddress( void ) const;

		void		setFd( int );
		void		setIpAddress( std::string );*/
};
