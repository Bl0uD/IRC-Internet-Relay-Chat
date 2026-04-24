/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChannelUtils.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdupuis <jdupuis@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 17:06:34 by jdupuis           #+#    #+#             */
/*   Updated: 2026/04/14 17:06:35 by jdupuis          ###   ########.fr       */
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
	return ( FindChannelWithTopic( newTopic ) != NULL );
}