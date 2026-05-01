/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SendTo.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdupuis <jdupuis@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/01 03:02:55 by jdupuis           #+#    #+#             */
/*   Updated: 2026/05/01 03:11:46 by jdupuis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../../includes/Server.hpp"

bool	SendToClient( Client *client, const std::string &message )
{
	if ( !client )
		return false;

	std::string	wire = message;
	size_t		total = 0;
	
	if ( ( wire.size() < 2 || wire.substr( wire.size() - 2 ) != CRLF )
		&& (wire.size() < 3 || wire.substr( wire.size() - 3 ) != CRLFNL ))
		wire += CRLF;
	while ( total < wire.size() )
	{
		ssize_t sent = send( client->getFd(), wire.c_str() + total, wire.size() - total, 0 );

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

void	Server::SendToChannel( Client *client, Channel *channel, const std::string &message, bool sendToMe )
{
	for ( std::set< int >::const_iterator it = channel->getClients().begin(); it != channel->getClients().end(); ++it )
	{
		Client *Member = FindClientWithFd( *it );
		if ( !Member )
			continue;
		if ( client != Member || sendToMe )
			SendToClient( Member, ":" + client->getPrefix() + " " + message );
	}
	return ;
}

void	Server::SendToAllClient( const std::string &message )
{
	int	size = this->_Clients.size();
	for ( int i = 0; i < size; i++ )
	{
		if ( IsRegistered( &this->_Clients[i] ) )
			this->respond( &this->_Clients[i], message );
	}
	return ;
}

void	Server::SendToAllMembers( Channel *channel, const std::string &message )
{
	for ( std::set< int >::const_iterator it = channel->getClients().begin(); it != channel->getClients().end(); ++it )
	{
		Client *member = FindClientWithFd( *it );
		if ( !member )
			continue;
		SendToClient( member, ":" + member->getPrefix() + " " + message );
	}
}