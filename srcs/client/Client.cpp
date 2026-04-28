/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdupuis <jdupuis@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 17:06:29 by jdupuis           #+#    #+#             */
/*   Updated: 2026/04/27 14:50:162 by jdupuis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../../includes/Server.hpp"

Client::~Client( void ) {}

Client::Client( void )
	: _fd( -1 ),
	  _ipAddress( "" ),
	  _username( "" ),
	  _nickname( "" ),
	  _hostname( "" ),
	  _password( "" ),
	  _registered( false ),
	  _isAuth( false ),
	  _isLog( false ),
	  _buffer( "" )
{}

Client::Client( std::string ipAddress )
	: _fd( -1 ),
	  _ipAddress( ipAddress ),
	  _username( "" ),
	  _nickname( "" ),
	  _hostname( "" ),
	  _password( "" ),
	  _registered( false ),
	  _isAuth( false ),
	  _isLog( false ),
	  _buffer( "" )
{}

Client::Client( Client const &copy )
{
	*this = copy;
}

Client	&Client::operator=( Client const &instance )
{
	if ( this != &instance )
	{
		this->_fd = instance._fd;
		this->_ipAddress = instance._ipAddress;
		this->_username = instance._username;
		this->_nickname = instance._nickname;
		this->_hostname = instance._hostname;
		this->_password = instance._password;
		this->_registered = instance._registered;
		this->_isAuth = instance._isAuth;
		this->_isLog = instance._isLog;
		this->_buffer = instance._buffer;
	}
	return ( *this );
}

int	Client::getFd( void ) const
{
	return ( this->_fd );
}

std::string	Client::getIpAddress( void ) const
{
	return ( this->_ipAddress );
}

std::string	Client::getUsername( void ) const
{
	return ( this->_username );
}

std::string	Client::getNickname( void ) const
{
	if ( this->_nickname.empty() )
		return ( this->_username );
	return ( this->_nickname );
}

std::string	Client::getPassword( void ) const
{
	return ( this->_password );
}

void	Client::appendBuffer( std::string data )
{
	this->_buffer += data;
}

bool	Client::getRegistered( void ) const
{
	return ( this->_registered );
}

bool	Client::getIsAuth( void ) const
{
	return ( this->_isAuth );
}
		
bool	Client::getIsLog( void ) const
{
	return ( this->_isLog );
}

void	Client::setFd( int fd )
{
	this->_fd = fd;
}

void	Client::setIpAddress( std::string add )
{
	this->_ipAddress = add;
}

void	Client::setUsername( std::string username )
{
	int i = 0;
	while ( username[i] == '#' )
		++i;
	this->_username = username.substr( i, username.size() - i );
}

void	Client::setNickname( std::string nickname )
{
	int i = 0;
	while ( nickname[i] == '#' )
		++i;
	this->_nickname = nickname.substr( i, nickname.size() - i );
}

void	Client::setPassword( std::string password )
{
	this->_password = password;
}

bool	SendToClient( int clientFd, const std::string &message )
{
	std::string	wire = message;
	size_t		total = 0;
	
	if ( (wire.size() < 2 || wire.substr( wire.size() - 2 ) != CRLF )
		&& ( wire.size() < 3 || wire.substr( wire.size() - 3 ) != CRLFNL ))
		wire += CRLF;
	while ( total < wire.size() )
	{
		ssize_t sent = send( clientFd, wire.c_str() + total, wire.size() - total, 0 );

		if ( sent > 0 )
			total += static_cast<size_t>( sent );
		else if ( sent == -1 && errno == EINTR )
			continue;
		else if ( sent == -1 && ( errno == EAGAIN || errno == EWOULDBLOCK ) )
			return false;
		else
			return false;
	}
	return true;
}

void	Client::setRegistered( bool status )
{
	this->_registered = status;
}

void	Client::setLog( bool status )
{
	this->_isLog = status;
}

void	Client::setAuth( bool status )
{
	this->_isAuth = status;
}