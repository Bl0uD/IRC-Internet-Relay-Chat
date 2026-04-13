# include "../../includes/Server.hpp"

bool	Server::_ServerStatus = false;

Server::~Server( void ) {}

Server::Server( void ) : _NextChannelId( 0 ) {}

Server::Server( char **av ) : _NextChannelId( 0 )
{
	std::stringstream ss( av[1] );
	int port;
	ss >> port;

	this->setPort( port );
	this->setPassword( av[2] );
}

Server::Server( Server const &copy )
{
	*this = copy;
}

Server	&Server::operator=( Server const &instance )
{
	if ( this != &instance )
	{
		this->_Port = instance._Port;
		this->_Password = instance._Password;
		this->_ServerStatus = instance._ServerStatus;
		this->_SocketFd = instance._SocketFd;
		this->_Clients = instance._Clients;
		this->_Channels = instance._Channels;
		this->_Fds = instance._Fds;
		this->_Nicknames = instance._Nicknames;
		this->_NextChannelId = instance._NextChannelId;
	}
	return ( *this );
}

int	Server::getPort( void ) const
{
	return ( this->_Port );
}

bool	Server::getStatus( void ) const
{
	return ( this->_ServerStatus );
}

std::string Server::getPassword( void ) const
{
	return ( this->_Password );
}

void	Server::setPort( int port )
{
	if ( port > 1023 && port < 49152 )
		this->_Port = port;
	else
		throw ERR_INVALID_PORT;
}

void	Server::setPassword( std::string pswrd )
{
	if ( !pswrd.empty() )
		this->_Password = pswrd;
	else
		throw ERR_INVALID_PASSWORD;
}
