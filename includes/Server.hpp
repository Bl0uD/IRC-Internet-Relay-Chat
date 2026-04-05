
# pragma once

#include "Include.hpp"
#include "Client.hpp"

class Client;

class Server
{
	private:
		int	_port;
		std::string _password;
		static bool	ON;
		int SocketFd;
		std::vector<Client> clients;
		std::vector<struct pollfd> fds;

	public:
		~Server();
		Server();
		Server( char ** );
		Server( const Server &copy );
		Server &operator=( const Server &instance );

		int			getPort( void ) const;
		std::string	getPassword( void ) const;

		void	setPort( int );
		void	setPassword( std::string );
		
		static void 	SignalHandler( int );
		void	Init( void );

		void SetSockOptions( void );
		void SetServSocket( void );

		void AcceptNewClient( void );
		void ReceiveNewData( int ClientFd );
		
};
