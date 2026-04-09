# include "../../includes/Server.hpp"

bool	Server::IsRegistered( int fd )
{
	for ( std::vector<Client>::const_iterator it = this->_Clients.begin(); it != this->_Clients.end(); ++it )
	{
		if ( it->getFd() == fd )
			return true;
	}
	return false;
}

int Server::FindClient( int ClientFd )
{
	int	i = 0;
	while ( this->_Clients[i].getFd() != ClientFd )
		i++;
	return ( i );
}