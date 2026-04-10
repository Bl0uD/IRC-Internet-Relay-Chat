# include "../../includes/Server.hpp"


bool	Server::IsOperator( int ClientFd )
{
	int Cli = FindClient( ClientFd );
	if ( !Cli.getOperator() )
	{
		this->SendToClient( ClientFd, "You're not a client operator." );
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
