/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChannelUtils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdupuis <jdupuis@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 17:06:34 by jdupuis           #+#    #+#             */
/*   Updated: 2026/04/26 15:07:10 by jdupuis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../../includes/Server.hpp"

bool	Server::InChannel( Client *client, Channel *channel )
{
	return ( channel->getClients().find( client->getFd() ) != channel->getClients().end() );
}

bool	Server::FindDuplicateTopic( std::string oldTopic, std::string newTopic )
{
	if ( newTopic == oldTopic )
		return false;
	return ( FindChannelWithName( newTopic ) != NULL );
}

Channel	*Server::FindChannelWithName( std::string name )
{
	for ( std::vector< Channel >::iterator it = this->_Channels.begin(); it != this->_Channels.end(); ++it )
	{
		if ( it->getName() == name )
			return ( &(*it) );
	}
	return ( NULL );
}