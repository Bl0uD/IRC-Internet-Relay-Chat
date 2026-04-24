/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SendTo.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdupuis <jdupuis@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 17:06:43 by jdupuis           #+#    #+#             */
/*   Updated: 2026/04/22 12:56:38 by jdupuis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../../includes/Server.hpp"

bool	Server::SendToClient( Client *client, const std::string &message )
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

void	Server::SendToChannel( Client *client, Channel *channel, const std::string &message )
{
	for ( std::set< int >::const_iterator it = channel->getClients().begin(); it != channel->getClients().end(); ++it )
	{
		int memberFd = *it;
		if ( memberFd == client->getFd() )
			continue ;
		Client *Member = FindClientWithFd( memberFd );
		if ( !Member )
			continue ;
		SendToClient( Member, CHANNEL_MESSAGE( channel->getTopic(), client->getNickname(), message ) );
	}
	SendToClient( client, GREEN + "Message sent to channel " + YELLOW + channel->getTopic() + GREEN + " members." + WHITE + CRLFNL );
	return ;
}

void	Server::SendToAllMembers( Channel *channel, const std::string &message )
{
	for ( std::set< int >::const_iterator it = channel->getClients().begin(); it != channel->getClients().end(); ++it )
	{
		Client *Member = FindClientWithFd( *it );
		if ( !Member )
			continue ;
		SendToClient( Member, message );
	}
	return ;
}

void	Server::SendToAllClient( const std::string &message )
{
	int	size = this->_Clients.size();
	for ( int i = 0; i < size; i++ )
	{
		if ( IsRegistered( &this->_Clients[i] ) )
			SendToClient( &this->_Clients[i], message );
	}
	return ;
}