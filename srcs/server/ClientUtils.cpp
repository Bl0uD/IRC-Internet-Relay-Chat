# include "../../includes/Server.hpp"


bool	Server::IsOperator( int ClientFd, int channelId )
{
	const std::set< int > &operators = this->_Channels[channelId].getOperators();
	return ( operators.find( ClientFd ) != operators.end() );
}

bool	Server::IsRegistered( int ClientFd )
{
	int id = FindClientId( ClientFd );

	if ( this->_Clients[id].getRegistered() )
		return true ;
	this->SendToClient( ClientFd, "Register first.\nSet an username with" \
		+ GREEN + " USER" \
		+ WHITE + " and a password with " \
		+ GREEN + " PASS." + WHITE );
	return false ;
}

int Server::FindClientId( int ClientFd )
{
	for ( size_t i = 0; i < this->_Clients.size(); ++i )
	{
		if ( this->_Clients[i].getFd() == ClientFd )
			return ( static_cast<int>( i ) );
	}
	return ( -1 );
}

int	Server::FindClientFd( std::string Nickname )
{
	size_t i = 0;
	
	while ( i < this->_Clients.size() )
	{
		if ( this->_Clients[i].getNickname() == Nickname )
			return ( this->_Clients[i].getFd() );
		i++;
	}
	return ( -1 );
}

std::string	Server::FindClientNickname( int ClientFd )
{
	for ( std::vector< Client >::const_iterator it = this->_Clients.begin(); it != this->_Clients.end(); ++it )
	{
		if ( it->getFd() == ClientFd )
			return ( it->getUsername() );
	}
	return ( "NotFound" );
}

bool	Server::InChannel( int ClientFd, int ChannelId )
{
	if ( ChannelId < 0 || ChannelId >= static_cast<int>( this->_Channels.size() ) )
		return ( false );
	const std::set< int > &clients = this->_Channels[ChannelId].getClients();
	return ( clients.find( ClientFd ) != clients.end() );
}
bool	Server::FindDuplicateTopic( std::string oldTopic, std::string newTopic )
{
	if ( newTopic != oldTopic
		&& this->_Topics.find( newTopic ) != this->_Topics.end() )
		return true;
	if ( oldTopic != "" )
		this->_Topics.erase( oldTopic );
	return false;
}
