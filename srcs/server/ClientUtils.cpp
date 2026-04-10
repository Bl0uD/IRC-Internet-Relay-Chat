# include "../../includes/Server.hpp"


bool	Server::IsOperator( int ClientFd )
{
	int i = FindClient( ClientFd );
	if ( i < 0 )
		return false ;
	if ( !this->_Clients[i].getOperator() )
	{
		this->SendToClient( ClientFd, "You're not a client operator." );
		return false ;
	}
	return true ;
}

bool	Server::IsRegistered( int ClientFd )
{
	int i = FindClient( ClientFd );

	if ( i < 0 )
		return false ;
	if ( !this->_Clients[i].getRegistered() )
	{
		this->SendToClient( ClientFd, "Register first." );
		return false ;
	}
	return true ;
}

int Server::FindClient( int ClientFd )
{
	for ( size_t i = 0; i < this->_Clients.size(); ++i )
	{
		if ( this->_Clients[i].getFd() == ClientFd )
			return ( static_cast<int>( i ) );
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
	return ( "NoUsername" );
}
