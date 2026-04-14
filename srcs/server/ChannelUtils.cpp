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