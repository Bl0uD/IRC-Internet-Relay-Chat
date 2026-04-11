#pragma once

#include "Include.hpp"

class Channel;

class Client
{
	private:
		int			_fd;
		std::string	_ipAddress;
		std::string _username;
		std::string _nickname;
		std::string _password;
		bool		_registered;

	public:
		~Client();
		Client();
		Client( std::string );
		Client( const Client &copy );
		Client &operator=( const Client &instance );

		int			getFd( void ) const;
		std::string	getIpAddress( void ) const;
		std::string	getUsername( void ) const;
		std::string	getNickname( void ) const;
		std::string getPassword( void ) const;

		bool		getRegistered( void ) const;

		void		setFd( int );
		void		setIpAddress( std::string );
		void		setUsername( std::string );
		void		setNickname( std::string );
		void		setPassword( std::string );

		void		setRegistered( bool );
};
