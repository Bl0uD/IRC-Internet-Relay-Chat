# include "../../includes/Server.hpp"

void	Server::SignalHandler( int signum )
{
	(void)signum;
	std::cout << RED << std::endl << "Signal Received!" << std::endl;
	_ServerStatus = false;
}

void	Server::Running( void )
{
	if ( poll( &this->_Fds[0], this->_Fds.size(), -1 ) == -1 )
	{
		if ( errno == EINTR && !this->_ServerStatus )
			throw SERVER_OFF;
		throw ERR_POLL;
	}
	for ( size_t i = 0; i < this->_Fds.size(); i++ )
	{
		if ( this->_Fds[i].revents & POLLIN )
		{
			if ( this->_Fds[i].fd == _SocketFd )
				AcceptNewClient();
			else
				ReceiveNewData( this->_Fds[i].fd );
		}
	}
}

void	Server::Init( void )
{
	this->SetServSocket();
	std::cout << SERVER_CONNECTED( _SocketFd );
	std::cout << WAITING_CONNECTION;
}

void	Server::SetSockOptions( void )
{
	int en = 1;
	struct sockaddr_in add;

	add.sin_family = AF_INET;
	add.sin_addr.s_addr = INADDR_ANY;
	add.sin_port = htons( this->_Port );

	if ( setsockopt( _SocketFd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof( en ) ) == -1 )
		throw ERR_SO_REUSEADDR;
	if ( fcntl( _SocketFd, F_SETFL, O_NONBLOCK ) == -1 )
		throw ERR_O_NONBLOCK;
	if ( bind( _SocketFd, (struct sockaddr *)&add, sizeof( add ) ) == -1 )
		throw ERR_BINDING_SOCKET;
	if ( listen( _SocketFd, SOMAXCONN ) == -1 )
		throw ERR_LISTEN;
}

void	Server::SetServSocket( void )
{
	struct pollfd new_cli;

	this->_SocketFd = socket( AF_INET, SOCK_STREAM, 0 );
	if ( _SocketFd == -1 )
		throw ERR_SOCKET_CREATION;
	this->SetSockOptions();
	new_cli.fd = _SocketFd;
	new_cli.events = POLLIN;
	new_cli.revents = 0;
	this->_Fds.push_back( new_cli );
	this->_ServerStatus = true;
}

void	Server::AcceptNewClient( void )
{
	Client			cli;
	struct sockaddr_in	cliadd;
	struct pollfd		NewPoll;
	socklen_t			len = sizeof( cliadd );

	int NewFd = accept( this->_SocketFd, (sockaddr *)&( cliadd ), &len );
	if ( NewFd == -1 )
	{
		std::cerr << ERR_ACCEPT_FAILED;
		return ;
	}
	if ( fcntl( NewFd, F_SETFL, O_NONBLOCK ) == -1 )
	{
		std::cerr << ERR_FCNTL_FAILED( NewFd );
		return ;
	}

	NewPoll.fd = NewFd;
	NewPoll.events = POLLIN;
	NewPoll.revents = 0;

	cli.setFd( NewFd );
	cli.setIpAddress( inet_ntoa( cliadd.sin_addr ) );
	_Clients.push_back( cli );
	_Fds.push_back( NewPoll );

	std::cout << NEW_CLIENT( NewFd );
	SendToClient( NewFd, MSG_NEW_CLIENT( NewFd ) );
}

void	Server::ReceiveNewData( int ClientFd )
{
	char Data[1024];
	memset( Data, 0, sizeof( Data ) );

	ssize_t bytes = recv( ClientFd, Data, sizeof( Data ) - 1, 0 );
	if ( bytes <= 0 )
	{
		std::cout << CLIENT_DISCONNECTED( ClientFd );
		close( ClientFd );
	}
	else
	{
		Data[bytes] = '\0';
		std::vector< std::string > Tokens = Parse( Data );
		ExecCommand( Tokens, ClientFd );
	}
}