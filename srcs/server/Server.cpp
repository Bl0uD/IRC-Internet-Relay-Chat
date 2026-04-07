# include "../../includes/Server.hpp"

bool	Server::_ServerStatus = false;

Server::~Server( void ) {}

Server::Server( void ) {}

Server::Server( char **av )
{
	std::stringstream ss( av[1] );
	int port;
	ss >> port;

	this->setPort( port );
	this->setPassword( av[2] );
}

Server::Server( Server const &copy )
{
	std::cout << "Server copy constructor called." << std::endl;
	*this = copy;
}

Server	&Server::operator=( Server const &instance )
{
	(void)instance;
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
	if ( port > 1023 && port < 49152 ) // conditions port valide
		this->_Port = port;
	else
		throw ERR_INVALID_PORT;
}

void	Server::setPassword( std::string pswrd )
{
	if ( !pswrd.empty() ) // conditions password valide
		this->_Password = pswrd;
	else
		throw ERR_INVALID_PASSWORD;
}


void	Server::SignalHandler( int signum )
{
	(void)signum;
	std::cout << RED << std::endl << "Signal Received!" << std::endl;
	_ServerStatus = false;
}

void	Server::Running( void )
{
		if ( poll( &this->_Fds[0], this->_Fds.size(), -1 ) == -1 ) // wait for an event
		{
			if ( errno == EINTR && !this->_ServerStatus ) // check if the poll crash was intentional or not
				throw SERVER_OFF;
			throw ERR_POLL;
		}
		for ( size_t i = 0; i < this->_Fds.size(); i++ )
		{
			if ( this->_Fds[i].revents & POLLIN ) // check if there is data to read
			{
				if ( this->_Fds[i].fd == _SocketFd )
					AcceptNewClient();
				else
					ReceiveNewData( this->_Fds[i].fd ); // receive new data from a registered client
			}
		}
}

void	Server::Init( void )
{
	this->SetServSocket();
	std::cout << SERVER_CONNECTED( _SocketFd );
	std::cout << WAITING_CONNECTION;
}
void Server::SetSockOptions( void )
{
	int en = 1;
	struct sockaddr_in	add;

	add.sin_family = AF_INET;
	add.sin_addr.s_addr = INADDR_ANY;
	add.sin_port = htons( this->_Port );

	if ( setsockopt(_SocketFd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1 ) // check if SO_REUSEADDR flag has been set
		throw ERR_SO_REUSEADDR;

	if ( fcntl(_SocketFd, F_SETFL, O_NONBLOCK) == -1 ) // check if NON_BLOCK flag has been set
		throw ERR_O_NONBLOCK ;

	if ( bind(_SocketFd, (struct sockaddr *)&add, sizeof(add)) == -1 ) // check if socket is bind (assigned) to address and port 
		throw ERR_BINDING_SOCKET;

	if ( listen(_SocketFd, SOMAXCONN) == -1 ) // check if socket is passive -> listen for incoming connections 
		throw ERR_LISTEN;

}

void Server::SetServSocket( void )
{
	struct pollfd	new_cli;

	this->_SocketFd = socket( AF_INET, SOCK_STREAM, 0 );
	if ( _SocketFd == -1 ) // check if socket has been created
		throw ERR_SOCKET_CREATION;
	this->SetSockOptions();
	new_cli.fd = _SocketFd;
	new_cli.events = POLLIN;
	new_cli.revents = 0;
	this->_Fds.push_back( new_cli );
	this->_ServerStatus = true;
}

void Server::AcceptNewClient( void )
{
	Client				cli; // create a new client
	struct sockaddr_in	cliadd;
	struct pollfd		NewPoll;
	socklen_t			len = sizeof( cliadd );

	int NewFd = accept( this->_SocketFd, (sockaddr *)&(cliadd), &len ); // accept the new client
	if ( NewFd == -1 )
	{
		std::cerr << ERR_ACCEPT_FAILED;
		return ;
	}

	if ( fcntl(NewFd, F_SETFL, O_NONBLOCK) == -1 ) // set the socket option (O_NONBLOCK) for non-blocking socket
	{
		std::cerr << ERR_FCNTL_FAILED( NewFd );
		return;
	}

	NewPoll.fd = NewFd; // add the client socket to the pollfd
	NewPoll.events = POLLIN; // set the event to POLLIN for reading data
	NewPoll.revents = 0; // set the revents to 0

	cli.setFd( NewFd ); // set the client file descriptor
	cli.setIpAddress( inet_ntoa((cliadd.sin_addr)) ); // convert the ip address to string and set it
	_Clients.push_back( cli ); // add the client to the vector of _Clients
	_Fds.push_back( NewPoll) ; // add the client socket to the pollfd

	std::cout << NEW_CLIENT( NewFd );
}


void Server::ReceiveNewData( int ClientFd )
{
	char	Data[1024]; // buffer for the received data
	memset( Data, 0, sizeof( Data ) ); // clear the buffer

	ssize_t bytes = recv( ClientFd, Data, sizeof( Data ) - 1 , 0 ); // receive the data

	if( bytes <= 0 ) // check if the client disconnected
	{
		std::cout << CLIENT_DISCONNECTED( ClientFd );
		//Clear_Clients(ClientFd); // clear the client
		close( ClientFd ); // close the client socket
	}
	else // print the received data
	{
		Data[bytes] = '\0';
		
		// std::cout << PRINT_DATA( ClientFd, Data );
		std::vector< std::string >	Tokens = Parse( Data );
		ExecCommand( Tokens, ClientFd );
		// here you can add your code to process the received data: parse, check, authenticate, handle the command, etc...
	}
}

bool Server::IsRegistered(int fd)
{
	for ( std::vector<Client>::const_iterator it = this->_Clients.begin(); it != this->_Clients.end(); ++it )
	{
		if ( it->getFd() == fd )
			return true;
	}
	return false;
}