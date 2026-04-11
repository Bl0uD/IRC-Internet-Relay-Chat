# include "../../includes/Server.hpp"


bool	Server::IsOperator( int ClientFd, int channelId )
{
	size_t i = 0;
	
	while ( i < this->_Channels[channelId].getOperators().size() )
	{	
		if ( this->_Channels[channelId].getOperators()[i] == ClientFd )		
			return true ;
		i++;
	}
	return false ;
}

bool	Server::IsRegistered( int ClientFd )
{
	int i = FindClientId( ClientFd );
	if ( !this->_Clients[i].getRegistered() )
	{
		this->SendToClient( ClientFd, "Register first." );
		return false ;
	}
	return true ;
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
	for ( size_t i = 0; this->_Channels[ChannelId].getClients()[i]; ++i )
	{
		if ( this->_Channels[ChannelId].getClients()[i] == ClientFd )
			return ( true );
	}
	return ( false );
}