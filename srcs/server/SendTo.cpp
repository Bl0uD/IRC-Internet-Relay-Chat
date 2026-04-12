# include "../../includes/Server.hpp"

bool	Server::SendToClient( int clientFd, const std::string &message )
{
	std::string	wire = message;
	size_t		total = 0;
	
	if ( wire.size() < 2 || wire.substr( wire.size() - 2 ) != CRLF )
		wire += CRLF;
	while ( total < wire.size() )
	{
		ssize_t sent = send( clientFd, wire.c_str() + total, wire.size() - total, 0 );

		if ( sent > 0 )
			total += static_cast<size_t>( sent );
		else if ( sent == -1 && errno == EINTR )
			continue;
		else if ( sent == -1 && ( errno == EAGAIN || errno == EWOULDBLOCK ) )
			return false;
		else
			return false;
	}
	return true;
}

int	Server::FindChannelId( std::string topic )
{
	for ( std::vector< Channel >::const_iterator it = this->_Channels.begin(); it != this->_Channels.end(); ++it )
	{
		if ( it->getTopic() == topic )
			return ( it->getId() );
	}
	return ( -1 );
}

void	Server::SendToChannel( int ClientFd, int ChannelId, const std::string &message )
{
	const std::set< int > &members = this->_Channels[ChannelId].getClients();
	for ( std::set< int >::const_iterator it = members.begin(); it != members.end(); ++it )
	{
		int memberFd = *it;
		if ( memberFd == ClientFd )
			continue ;
		SendToClient( memberFd, message );
	}
	SendToClient( ClientFd, "Message correctly send to channel members\n.");
	return ;
}

void	Server::SendToAllMembers( int ChannelId, const std::string &message )
{
	const std::set< int > &members = this->_Channels[ChannelId].getClients();
	for ( std::set< int >::const_iterator it = members.begin(); it != members.end(); ++it )
		SendToClient( *it, message );
	return ;
}

void	Server::SendToAllClient( const std::string &message )
{
	int	size = this->_Clients.size();
	for ( int i = 0; i < size; i++ )
		SendToClient( this->_Clients[i].getFd(), message );
	return ;
}