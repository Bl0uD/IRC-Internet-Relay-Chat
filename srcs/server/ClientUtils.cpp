/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientUtils.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdupuis <jdupuis@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 17:06:36 by jdupuis           #+#    #+#             */
/*   Updated: 2026/04/14 17:06:37 by jdupuis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../../includes/Server.hpp"

bool	Server::IsOperator( Client *client, Channel *channel )
{
	return ( channel->getOperators().find( client->getFd() ) != channel->getOperators().end() );
}

bool	Server::IsRegistered( Client *client )
{
	if ( client->getRegistered() )
		return true ;
	this->SendToClient( client, GREEN + "Register first.\nSet a username with " \
		+ YELLOW + "USER" \
		+ GREEN + " and a password with " \
		+ YELLOW + "PASS" + GREEN + "." + WHITE );
	return false ;
}

Client	*Server::FindClientWithNickname( std::string Nickname )
{
	for ( std::vector<Client>::iterator it = this->_Clients.begin(); it != this->_Clients.end(); ++it )
	{
		if ( it->getNickname() == Nickname )
			return ( &(*it) );
	}

	return ( NULL );
}

Client	*Server::FindClientWithFd( int fd )
{
	for ( std::vector<Client>::iterator it = this->_Clients.begin(); it != this->_Clients.end(); ++it )
	{
		if ( it->getFd() == fd )
			return ( &(*it) );
	}

	return ( NULL );
}

Channel	*Server::FindChannelWithTopic( std::string topic )
{
	for ( std::vector< Channel >::iterator it = this->_Channels.begin(); it != this->_Channels.end(); ++it )
	{
		if ( it->getTopic() == topic )
			return ( &(*it) );
	}
	return ( NULL );
}