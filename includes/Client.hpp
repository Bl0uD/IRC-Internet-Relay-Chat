#pragma once

#include "Include.hpp"

class Client
{
	private:
		int			_fd;
		std::string	_ipAddress;

		std::string _username;
		std::string _nickname;

		//bool _isOperator;
		//bool _registered;
		//std::vector<std::string> ChannelsInvite;

	public:
		~Client();
		Client();
		Client( std::string );
		Client( const Client &copy );
		Client &operator=( const Client &instance );

		int			getFd( void ) const;
		std::string	getIpAddress( void ) const;

		void		setFd( int );
		void		setIpAddress( std::string );
		void		setUsername( std::string );
		void		setNickname( std::string );
};
