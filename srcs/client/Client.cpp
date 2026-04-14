/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdupuis <jdupuis@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 17:06:29 by jdupuis           #+#    #+#             */
/*   Updated: 2026/04/14 17:06:30 by jdupuis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../../includes/Client.hpp"

Client::~Client( void ) {}

Client::Client( void )
	: _fd( -1 ),
	  _ipAddress( "" ),
	  _username( "" ),
	  _nickname( "" ),
	  _password( "" ),
	  _registered( false )
{}

Client::Client( std::string ipAddress )
	: _fd( -1 ),
	  _ipAddress( ipAddress ),
	  _username( "" ),
	  _nickname( "" ),
	  _password( "" ),
	  _registered( false )
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
		this->_password = instance._password;
		this->_registered = instance._registered;
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

bool	Client::getRegistered( void ) const
{
		// send au client message : "Register first." 
	return ( this->_registered );
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
	this->_username = username;
}

void	Client::setNickname( std::string nickname )
{
	this->_nickname = nickname;
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

void	Client::setRegistered( bool regis )
{
	this->_registered = regis;
	if ( this->_registered )
		SendToClient( this->getFd(), REGISTERED( "registered" ) );
	else
		SendToClient( this->getFd(), REGISTERED( "unregistered" ) );
}
