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

void	Server::SendToChannel( int ClientFd, std::vector<std::string> Tokens )
{
	int ChannelId = FindChannelId( Tokens[1] );
	if ( ChannelId == -1 )
	{
		this->SendToClient( ClientFd, ERR_INEXISTANT_CHANNEL( Tokens[1] ) );
		return ;
	}
	if ( !InChannel( ClientFd, ChannelId ) )
	{
		ERR_NOT_IN_CHANNEL( Tokens[1] );
		return ;
	}
	std::string message;
	for ( size_t i = 2; i < Tokens.size(); ++i )
	{
		if ( i > 2 )
			message += " ";
		message += Tokens[i];
	}

	int	size = this->_Channels[ChannelId].getClients().size();
	for ( int i = 0; i < size; i++ )
	{
		if ( this->_Channels[ChannelId].getClients()[i] == ClientFd )
			i++;
		SendToClient( this->_Channels[ChannelId].getClients()[i], message );
	}
	SendToClient( ClientFd, "Message correctly send to channel members\n.");
	return ;
}

void	Server::SendToAllMembers( int ChannelId, std::string message )
{
	int	size = this->_Channels[ChannelId].getClients().size();
	for ( int i = 0; i < size; i++ )
		SendToClient( this->_Channels[ChannelId].getClients()[i], message );
	return ;
}

void	Server::SendToAllClient( const std::string &message )
{
	int	size = this->_Clients.size();
	for ( int i = 0; i < size; i++ )
		SendToClient( this->_Clients[i].getFd(), message );
	return ;
}