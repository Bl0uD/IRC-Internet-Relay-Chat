/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerRunning.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdupuis <jdupuis@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/01 03:03:49 by jdupuis           #+#    #+#             */
/*   Updated: 2026/05/03 16:36:57 by jdupuis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../../includes/Server.hpp"

void	Server::SignalHandler( int signum )
{
	(void)signum;
	std::cout << RED << std::endl << "Signal Received!" << WHITE << std::endl;
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
			{
				Client	*client = FindClientWithFd( this->_Fds[i].fd );
				if ( !client )
				{
					this->_Fds.erase( this->_Fds.begin() + i );
					if ( i > 0 )
						i--;
					continue ;
				}
				ReceiveNewData( client );
			}
		}
	}
}

void	Server::Init( void )
{
	this->SetServSocket();
	std::cout << SERVER_CONNECTED( this->_SocketFd, this->getPort() );
	std::cout << WAITING_CONNECTION;
}

void	Server::SetSockOptions( void )
{
	int					en = 1;
	struct sockaddr_in	add;

	add.sin_family = AF_INET;
	add.sin_addr.s_addr = INADDR_ANY;
	add.sin_port = htons( this->_Port );

	if ( setsockopt( _SocketFd, SOL_SOCKET, SO_REUSEADDR, &en, sizeof( en ) ) == -1 )
	{
		close( this->_SocketFd );
		throw ERR_SO_REUSEADDR;
	}
	if ( fcntl( _SocketFd, F_SETFL, O_NONBLOCK ) == -1 )
	{
		close( this->_SocketFd );
		throw ERR_O_NONBLOCK;
	}
	if ( bind( _SocketFd, (struct sockaddr *)&add, sizeof( add ) ) == -1 )
	{
		close( this->_SocketFd );
		throw ERR_BINDING_SOCKET;
	}
	if ( listen( _SocketFd, SOMAXCONN ) == -1 )
	{
		close( this->_SocketFd );
		throw ERR_LISTEN;	
	}
}

void	Server::SetServSocket( void )
{
	struct pollfd	new_cli;

	this->_SocketFd = socket( AF_INET, SOCK_STREAM, 0 );
	if ( _SocketFd == -1 )
		throw ERR_SOCKET_CREATION;
	this->SetSockOptions();
	new_cli.fd = _SocketFd;
	new_cli.events = POLLIN;
	new_cli.revents = 0;
	this->_Fds.push_back( new_cli );
	this->_ServerStatus = true;
	
	// Récupérer l'adresse réelle liée au socket
	sockaddr_in	actualAddr;
	socklen_t	addrLen = sizeof(actualAddr);
	if ( getsockname(this->_SocketFd, (struct sockaddr *)&actualAddr, &addrLen) == -1 )
	{
		close( this->_SocketFd );
		throw std::runtime_error( "Error: getsockname failed" );
	}

	// Résolution du nom de l'hôte
	char	hostBuffer[NI_MAXHOST];
	int		ret = getnameinfo( (struct sockaddr *)&actualAddr, sizeof(actualAddr),
							hostBuffer, sizeof(hostBuffer), NULL, 0, 0 );
	if ( ret != 0 )
	{
		close( this->_SocketFd );
		throw std::runtime_error( std::string("Error: ") + gai_strerror(ret) );
	}

	this->_hostName = std::string( hostBuffer );
}

void	Server::AcceptNewClient( void )
{
	Client				client;
	struct sockaddr_in	clientAdd;
	struct pollfd		NewPoll;
	socklen_t			len = sizeof( clientAdd );

	int	NewFd = accept( this->_SocketFd, (sockaddr *)&( clientAdd ), &len );
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

	client.setFd( NewFd );
	client.setIpAddress( inet_ntoa( clientAdd.sin_addr ) );
	this->_Clients.push_back( client );
	_Fds.push_back( NewPoll );

	std::cout << NEW_CLIENT( client.getFd() );
}

void	Server::RemoveClient( Client *client )
{
	int			fd = client->getFd();
	std::string	username = client->getUsername();
	std::string	nickname = client->getNickname();

	std::cout << CLIENT_DISCONNECTED( fd );
	close( fd );
	SendToAllClient( RPL_QUIT( nickname, "Leaving" ) );
	for ( std::vector< Channel >::iterator it = this->_Channels.begin(); it != this->_Channels.end(); ++it )
	{
		it->removeClient( client );
		it->removeOperator( fd );
		if ( it->getOperators().empty() && !it->getClients().empty() )
		{
			int newOpFd = *it->getClients().begin();
			Client *newOp = FindClientWithFd( newOpFd );
			if ( newOp )
				it->setOperator( this, client, newOp, '+' );
		}
		it->removePendingClient( fd );
	}
	PruneEmptyChannels();

	for ( std::vector< Client >::iterator it = this->_Clients.begin(); it != this->_Clients.end(); ++it )
	{
		if ( it->getFd() == fd )
		{
			if ( nickname != "" )
				this->_ClientNames.erase( nickname );
			this->_Clients.erase( it );
			break ;
		}
	}

	for ( std::vector< struct pollfd >::iterator it = this->_Fds.begin(); it != this->_Fds.end(); ++it )
	{
		if ( it->fd == fd )
		{
			this->_Fds.erase( it );
			break ;
		}
	}
}

void	Server::ReceiveNewData( Client *client )
{
	if ( !client )
		return ;

	char	buffer[BUFFER_SIZE];
	memset( buffer, 0, sizeof( buffer ) );

	ssize_t	bytes = recv( client->getFd() , buffer, sizeof( buffer ) - 1, 0 );
	if ( bytes <= 0 )
		RemoveClient( client );
	else
	{
		buffer[bytes] = '\0';
		client->appendBuffer( buffer );
		std::string	message;
		while ( (message = client->extractNextMessage()) != "" )
		{
			this->Parse( message );
			ExecCommand( client );
		}
	}
}