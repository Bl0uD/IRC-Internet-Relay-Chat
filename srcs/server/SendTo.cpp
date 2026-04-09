# include "../../includes/Server.hpp"


bool	Server::SendToClient( int clientFd, const std::string &message )
{
	std::string	wire = message;
	size_t		total = 0;
	
	if ( wire.size() < 1 || wire.substr( wire.size() - 1 ) != "\r" )
		wire += "\r";
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

bool	Server::SendToAllClient( int clientFd, const std::string &message )
{
	int	size = this->_Clients.size();
	for ( int i = 0; i < size; i++ )
	{
		if ( this->_Clients[i].getFd() == clientFd )
			i++;
		SendToClient( this->_Clients[i].getFd(), message );
	}
	return true;
}