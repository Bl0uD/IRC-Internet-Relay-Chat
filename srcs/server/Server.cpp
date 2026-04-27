
# include "../../includes/Server.hpp"

bool	Server::_ServerStatus = false;

Server::~Server( void ) {}

Server::Server( void )
	: _Port( 0 ),
	  _SocketFd( -1 ),
	  _NextChannelId( 0 ),
	  _Password( "" ),
	  _hostName( "" ),
	  _Clients(),
	  _Fds(),
	  _Channels(),
	  _ClientNames(),
	  _Usernames(),
	  _Topics(),
	  _parsedMessages()
{}

Server::Server( char **av )
	: _Port( 0 ),
	  _SocketFd( -1 ),
	  _NextChannelId( 0 ),
	  _Password( "" ),
	  _hostName( "" ),
	  _Clients(),
	  _Fds(),
	  _Channels(),
	  _ClientNames(),
	  _Usernames(),
	  _Topics(),
	  _parsedMessages()
{
	this->setPort( ft_atoi( av[1] ) );
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
		this->_hostName = instance._hostName;
		this->_Clients = instance._Clients;
		this->_Channels = instance._Channels;
		this->_Fds = instance._Fds;
		this->_ClientNames = instance._ClientNames;
		this->_Usernames = instance._Usernames;
		this->_Topics = instance._Topics;
		this->_NextChannelId = instance._NextChannelId;
		this->_parsedMessages = instance._parsedMessages;
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
