# include "../../includes/Server.hpp"

bool	Server::InChannel( Client *client, Channel *channel )
{
	return ( channel->getClients().find( client->getFd() ) != channel->getClients().end() );
}

bool	Server::FindDuplicateTopic( std::string oldTopic, std::string newTopic )
{
	if ( newTopic != oldTopic
		&& this->_Topics.find( newTopic ) != this->_Topics.end() )
		return false;
	if ( oldTopic != "" )
		this->_Topics.erase( oldTopic );
	return true;
}