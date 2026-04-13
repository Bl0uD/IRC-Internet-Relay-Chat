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

		void		SetUsername( std::vector<std::string> Tokens, Client *client );
		void		SetNickname( std::vector<std::string> Tokens, Client *client );
		void		SetPassword( std::vector<std::string> Tokens, Client *client );
		void		ChangeTopic( std::vector<std::string> Tokens, Client *client );
		void		KickClient( std::vector<std::string> Tokens, Client *client );
		void		InviteClient( std::vector<std::string> Tokens, Client *client );
		void		JoinChannel( std::vector<std::string> Tokens, Client *client );
		void		ChangeMode( std::vector<std::string> Tokens, Client *client );
		void		SendPrivMsg( std::vector<std::string> Tokens, Client *client );
		void		ChannelMessage( std::vector< std::string > Tokens, Client *client );
		void		ChannelList( Client *client );
		void		ExecCommand( std::vector<std::string> Tokens, Client *client );
		
		void		AcceptNewClient( void );
		void		ReceiveNewData( Client *client );

		bool		IsOperator( Client *client, Channel *channel );
		bool		IsRegistered( Client *client );

		void		SendToAllClient( const std::string &message );
		void		SendToChannel( Client *client, Channel *channel, const std::string &message );
		bool		SendToClient( Client *client, const std::string &message );
		void		SendToAllMembers( Channel *channel, const std::string &message );
		bool		InChannel( Client *client, Channel *channel );
		void		SetRemovePassword( Channel *channel, std::string password );
		bool		FindDuplicateTopic( std::string oldTopic, std::string newTopic );
		bool		IsChannelFull( Channel *channel );
		Client		*FindClientWithFd( int );
		Channel		*FindChannelWithTopic( std::string topic );
		Client		*FindClientWithNickname( std::string Nickname );
		bool		GiveTakeOperatorGrade( Channel *channel, Client *target );
		void		SetRemoveUserLimitation( Channel *channel, std::string	Limitation );
};