/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: norabino <norabino@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/31 14:12:00 by jdupuis           #+#    #+#             */
/*   Updated: 2026/04/02 18:01:111 by norabino         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Server.hpp"

Server::~Server( void )
{
	std::cout << "Server destructor called." << std::endl;
}

Server::Server( void )
{
	std::cout << "Server default constructor called." << std::endl;
	
}

Server::Server( char **av )
{
	int port;
	std::string password;
	std::stringstream	ss( av[1] );

	ss >> port;
	password = av[2];
	this->setPort( port );
	this->setPassword( password );
}

Server::Server( Server const &copy )
{
	std::cout << "Server copy constructor called." << std::endl;
	*this = copy;
}

Server	&Server::operator=( Server const &instance )
{
	
	return ( *this );
}

int Server::getPort( void ) const
{
	return ( this->_port );
}

std::string Server::getPassword( void ) const
{
	return ( this->_password );
}

void	Server::setPort( int port )
{
	if ( port > 1023 && port < 49152 ) // conditions port valide
		this->_port = port;
	//else
		//std::cout << "Invalid port.\nServer reading port set to 0 (default value).\n";
		//throw exception
}

void	Server::setPassword( std::string pswrd )
{
	if ( !pswrd.empty() ) // conditions password valide
		this->_password = pswrd;
	//else
		//throw exception
}