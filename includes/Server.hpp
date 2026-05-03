/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdupuis <jdupuis@student.42perpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 17:06:20 by jdupuis           #+#    #+#             */
/*   Updated: 2026/05/03 16:13:17 by jdupuis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

# include "Include.hpp"
# include "Client.hpp"
# include "Channel.hpp"
# include "Parser.hpp"

class Client;

class Server
{
	private:
		int                                _Port;
		int                                _SocketFd;
		int                                _NextChannelId;
		static bool                         _ServerStatus;
		std::string                         _Password;
		std::string                         _hostName;
		std::vector< Client >               _Clients;
		std::vector< struct pollfd >        _Fds;
		std::vector< Channel >              _Channels;
		std::set< std::string >             _ClientNames;
		std::set< std::string >             _Topics;
		std::vector<Parser>                 _parsedMessages;

	public:
		~Server();
		Server();
		Server( char ** );
		Server( const Server &copy );
		Server &operator=( const Server &instance );
	
		static void						SignalHandler( int );
		std::vector< struct pollfd >	&getPollFds();

		int								getPort( void ) const;
		void							setPort( int );
		std::string						getPassword( void ) const;
		void							setPassword( std::string );
		bool							getStatus( void ) const;
		std::string						getPrefix();

		void							Init( void );
		void							Running( void );
		void							SetServSocket( void );
		void							SetSockOptions( void );
		void							Parse( std::string );

		/* Command handlers */
		void    					    ChangeMode( Client *client, Parser parser );
		void							ChangeTopic( Client *client, Parser parser );
		void    					    ExecCommand( Client *client );
		void							InviteClient( Client *client, Parser parser );
		void							JoinChannel( Client *client, Parser parser );
		void							KickClient( Client *client, Parser parser );
		void							SendPrivMsg( Client *client, Parser parser );
		void							SendWelcome( Client *client );
		void							cmdCap( Client *client, Parser parser );
		void							cmdPing( Client *client, Parser parser );
		void							cmdPart( Client *client, Parser cmd );
		void							cmdQuit( Client *client, Parser parser );

		/* Client management */
		void							AcceptNewClient( void );
		void							RemoveClient( Client *client );
		void							ReceiveNewData( Client *client );
		bool							IsRegistered( Client *client );

		/* Channel management */
		void							SendToChannel( Client *client, Channel *channel, const std::string &message, bool sendToMe );
		void							SendToAllMembers( Channel *channel, const std::string &message );
		bool							InChannel( Client *client, Channel *channel );
		bool							IsChannelFull( Channel *channel );
		bool							FindDuplicateTopic( std::string oldTopic, std::string newTopic );
		void							PruneEmptyChannels( void );
		Channel							*FindChannelWithName( std::string topic );

		void							SendToAllClient( const std::string &message );
		Client							*FindClientWithFd( int );
		Client							*FindClientWithNickname( std::string Nickname );
		void							respond( Client *client, std::string message );
		void							SetUsername( Client *client, Parser parser );
		void							SetNickname( Client *client, Parser parser );
		void							SetPassword( Client *client, Parser parser );
};

bool	SendToClient( Client *client, const std::string &message );
bool	IsOperator( Client *client, Channel *channel );

int		ft_atoi( std::string str );