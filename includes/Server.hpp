# pragma once

# include "Include.hpp"
# include "Client.hpp"

class Client;

class Server
{
	private:
		int							_Port;
		std::string					_Password;
		static bool					_ServerStatus;
		int							_SocketFd;
		std::vector<Client>			_Clients;
		std::vector<struct pollfd>	_Fds;
		std::vector< std::pair<int, std::string> >	_Directory;

	public:
		~Server();
		Server();
		Server( char ** );
		Server( const Server &copy );
		Server		&operator=( const Server &instance );

		int			getPort( void ) const;
		std::string	getPassword( void ) const;
		bool		getStatus( void ) const;

		void		setPort( int );
		void		setPassword( std::string );
		
		static void	SignalHandler( int );
		void		Init( void );
		void		Running( void );

		void		SetSockOptions( void );
		void		SetServSocket( void );

		std::vector< std::string >	Parse( char *Data );
		void		SetUsername( std::vector<std::string> Tokens, int ClientFd );
		void		SetNickname( std::vector<std::string> Tokens, int ClientFd );
		void		ChangeTopic( std::vector<std::string> Tokens, int ClientFd );
		void		KickClient( std::vector<std::string> Tokens, int ClientFd );
		void		InviteClient( std::vector<std::string> Tokens, int ClientFd );
		void		JoinChannel( std::vector<std::string> Tokens, int ClientFd );
		void		ChangeMode( std::vector<std::string> Tokens, int ClientFd );
		void		SendPrivMsg( std::vector<std::string> Tokens, int ClientFd );
		void		ExecCommand( std::vector<std::string> Tokens, int ClientFd );
		bool		IsRegistered( int );
		
		void		AcceptNewClient( void );
		void		ReceiveNewData( int );
		int			FindClient( int );
		
};
