# include "../../includes/Server.hpp"

bool Server::ON = false;

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

int Server::getPort( void ) const
{
	return ( this->_port );
}

std::string Server::getPassword( void ) const
{
	return ( this->_password );
}

void	Server::setPort( int port )
{
	if ( port > 1023 && port < 49152 ) // conditions port valide
		this->_port = port;
	else
		throw ERR_INVALID_PORT;
}

void	Server::setPassword( std::string pswrd )
{
	if ( !pswrd.empty() ) // conditions password valide
		this->_password = pswrd;
	else
		throw ERR_INVALID_PASSWORD;
}


void Server::SignalHandler(int signum)
{
	(void)signum;
	std::cout << RED << std::endl << "Signal Received!" << std::endl;
	ON = false;
}

void	Server::Init( void )
{
	this->SetServSocket();
	std::cout << SERVER_CONNECTED( SocketFd );

	std::cout << WAITING_CONNECTION;

	while ( this->ON )
	{

		if ( poll( &this->fds[0], this->fds.size(), -1 ) == -1 ) // wait for an event
		{
			if ( errno == EINTR && !this->ON ) // check if the poll crash was intentional or not
				throw SERVER_OFF;
			throw ERR_POLL;
		}

		for ( size_t i = 0; i < this->fds.size(); i++ )
		{
			if ( this->fds[i].revents & POLLIN ) // check if there is data to read
			{
				if ( this->fds[i].fd == SocketFd )
					AcceptNewClient();
				else
					ReceiveNewData( this->fds[i].fd ); // receive new data from a registered client
			}
		}
	}
	//CloseFds(); // close the file descriptors when the server stops*/
}
void Server::SetSockOptions( void )
{
	int en = 1;
	struct sockaddr_in	add;

	add.sin_family = AF_INET;
	add.sin_addr.s_addr = INADDR_ANY;
	add.sin_port = htons( this->_port );

	if( setsockopt(SocketFd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof(en)) == -1 ) // check if SO_REUSEADDR flag has been set
		throw ERR_SO_REUSEADDR;

	if ( fcntl(SocketFd, F_SETFL, O_NONBLOCK) == -1 ) // check if NON_BLOCK flag has been set
		throw ERR_O_NONBLOCK ;

	if ( bind(SocketFd, (struct sockaddr *)&add, sizeof(add)) == -1 ) // check if socket is bind (assigned) to address and port 
		throw ERR_BINDING_SOCKET;

	if ( listen(SocketFd, SOMAXCONN) == -1 ) // check if socket is passive -> listen for incoming connections 
		throw ERR_LISTEN;

}

void Server::SetServSocket( void )
{
	struct pollfd	new_cli;

	this->SocketFd = socket( AF_INET, SOCK_STREAM, 0 );
	if( SocketFd == -1 ) // check if socket has been created
		throw ERR_SOCKET_CREATION;
	this->SetSockOptions();

	new_cli.fd = SocketFd;
	new_cli.events = POLLIN;
	new_cli.revents = 0;
	this->fds.push_back(new_cli);
	this->ON = true;
}

void Server::AcceptNewClient( void )
{
	Client				cli; // create a new client
	struct sockaddr_in	cliadd;
	struct pollfd		NewPoll;
	socklen_t			len = sizeof( cliadd );

	int NewFd = accept( this->SocketFd, (sockaddr *)&(cliadd), &len ); // accept the new client
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
	clients.push_back( cli ); // add the client to the vector of clients
	fds.push_back( NewPoll) ; // add the client socket to the pollfd

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
		//ClearClients(ClientFd); // clear the client
		close( ClientFd ); // close the client socket
	}
	else // print the received data
	{
		Data[bytes] = '\0';
		std::cout << PRINT_DATA( ClientFd, Data );
		// here you can add your code to process the received data: parse, check, authenticate, handle the command, etc...
	}
}