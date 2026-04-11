# pragma once

# include "Include.hpp"
# include "Client.hpp"
# include "Channel.hpp"

class Client;

class Server
{
	private:
		int											_Port;
		int											_SocketFd;
		int											_NextChannelId;
		static bool									_ServerStatus;
		std::string									_Password;
		std::vector< Client >						_Clients;
		std::vector< std::pair<int, std::string> >	_ChannelDirectory;
		std::vector< struct pollfd >				_Fds;
		std::vector< Channel >						_Channels;
		std::set< std::string >						_Nicknames;
		std::set< std::string >						_Topics;

	public:
		~Server();
		Server();
		Server( char ** );
		Server( const Server &copy );
		Server		&operator=( const Server &instance );

		int			getPort( void ) const;
		void		setPort( int );
		std::string	getPassword( void ) const;
		void		setPassword( std::string );
		bool		getStatus( void ) const;


		static void	SignalHandler( int );
		void		Init( void );
		void		Running( void );

		void		SetSockOptions( void );
		void		SetServSocket( void );

		std::vector< std::string >	Parse( char *Data );

		void		SetUsername( std::vector<std::string> Tokens, int ClientFd );
		void		SetNickname( std::vector<std::string> Tokens, int ClientFd );
		void		SetPassword( std::vector<std::string> Tokens, int ClientFd );
		void		ChangeTopic( std::vector<std::string> Tokens, int ClientFd );
		void		KickClient( std::vector<std::string> Tokens, int ClientFd );
		void		InviteClient( std::vector<std::string> Tokens, int ClientFd );
		void		JoinChannel( std::vector<std::string> Tokens, int ClientFd );
		void		ChangeMode( std::vector<std::string> Tokens, int ClientFd );
		void		SendPrivMsg( std::vector<std::string> Tokens, int ClientFd );
		void		ExecCommand( std::vector<std::string> Tokens, int ClientFd );
		void		ChannelList( int ClientFd );
		
		void		AcceptNewClient( void );
		void		ReceiveNewData( int );

		int			FindChannelId( std::string topic );
		int			FindClientId( int );
		std::string	FindClientNickname( int ClientFd );
		int			FindClientFd( std::string Nickname );
		bool		IsOperator( int ClientFd, int channelId );
		bool		IsRegistered( int ClientFd );

		void		SendToAllClient( const std::string &message );
		void		SendToChannel( int clientFd, std::vector<std::string> Tokens );
		bool		SendToClient( int clientFd, const std::string &message );
		void		SendToAllMembers( int ChannelId , std::string message );
		bool		InChannel( int ClientFd, int ChannelId );


};
