

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

bool	Channel::isClientInChannel( Client *client )
{
	return ( this->getClients().find( client->getFd() ) != this->getClients().end() );
}

bool	Channel::isClientAdmin( Client *client )
{
	return ( this->getOperators().find( client->getFd() ) != this->getOperators().end() );
}

std::string	Channel::getModsForReply() const
{
	std::set<char>::iterator it;
	std::string reply = "";
	if (!this->_mods.empty())
		reply += '+';
	for (it = this->_mods.begin(); it != this->_mods.end(); ++it)
		reply += *it;
	return reply;
}

void	Server::PruneEmptyChannels( void )
{
	for ( std::vector< Channel >::iterator it = this->_Channels.begin(); it != this->_Channels.end(); )
	{
		if ( it->getClients().empty() )
			it = this->_Channels.erase( it );
		else
			++it;
	}
}

void	Channel::setKey(Server *server, Client *client,  char sign, std::string _key)
{
	if ( sign == '-' )
	{
		if ( this->_mods.count('k') == 1 )
		{
			this->_key.clear();
			this->setMods( server, client, sign, 'k' );
			server->SendToChannel( client, this, RPL_MODE(this->_name, "-k "), true );
		}
	}
	else
	{
		if ( _key != this->_key )
		{
			this->_key = _key;
			this->setMods( server, client, sign, 'k' );
			server->SendToChannel( client, this, RPL_MODE(this->_name, "+k " + _key), true );
		}
	}
}

void	Channel::setUserLimitation( Server *server, Client *client, char sign, std::string limit )
{
	if ( sign == '-' )
	{
		if ( this->_mods.count('l') == 1 )
		{
			this->_userLimitation = 0;
			this->setMods( server, client, sign, 'l' );
			server->SendToChannel( client, this, RPL_MODE( this->_name, "-l " ), true );
		}
	}
	else
	{
		int lim = ft_atoi( limit );
		bool valid = !limit.empty();
		for ( size_t i = 0; valid && i < limit.size(); ++i )
		{
			if ( !std::isdigit( static_cast<unsigned char>( limit[i] ) ) )
				valid = false;
		}
		if ( valid && lim >= 0 && lim != this->_userLimitation  )
		{
			this->_userLimitation = lim;
			this->setMods( server, client, sign, 'l' );
			server->SendToChannel( client, this, RPL_MODE( this->_name, "+l " + limit), true );
		}
	}
}
