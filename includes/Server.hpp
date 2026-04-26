/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdupuis <jdupuis@student.42perpignanperpignan.fr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/14 17:06:20 by jdupuis           #+#    #+#             */
/*   Updated: 2026/04/23 18:25:2541 by jdupuis          ###   ########.fr       */
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
		int											_Port;
		int											_SocketFd;
		int											_NextChannelId;
		static bool									_ServerStatus;
		std::string									_Password;
		std::string									_hostName;
		std::vector< Client >						_Clients;
		std::vector< struct pollfd >				_Fds;
		std::vector< Channel >						_Channels;
		std::set< std::string >						_Nicknames;
		std::set< std::string >						_Usernames;
		std::set< std::string >						_Topics;
		std::vector<Parser>							_parsedMessages;
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
		std::string	getPrefix();

		static void	SignalHandler( int );
		void		Init( void );
		void		Running( void );
		void		SetServSocket( void );

		void		Parse( std::string );

		void		SetSockOptions( void );

		void		SetUsername( Client *client, Parser parser );
		void		SetNickname( Client *client, Parser parser );
		void		SetPassword( Client *client, Parser parser );
		void		ChangeTopic( Client *client, Parser parser );
		void		KickClient( Client *client, Parser parser );
		void		InviteClient( Client *client, Parser parser );
		void		JoinChannel( Client *client, Parser parser );
		void		ChangeMode( Client *client, Parser parser );
		void		SendPrivMsg( Client *client, Parser parser );
		void		ChannelMessage( Client *client, Parser parser );
		void		ChannelList( Client *client, Parser parser );
		void		Help( Client *client, Parser parser );

		void		ExecCommand( Client *client );
		
		void		AcceptNewClient( void );
		void		RemoveClient( Client *client );
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
		Channel		*FindChannelWithName( std::string topic );
		Client		*FindClientWithNickname( std::string Nickname );
		bool		GiveTakeOperatorGrade( Channel *channel, Client *target );
		void		SetRemoveUserLimitation( Channel *channel, std::string	Limitation );
		void		PruneEmptyChannels( void );
		void		respond( Client *client, std::string message );
		void		cmdQuit( Client *client, Parser parser );
		void		cmdCap( Client *client, Parser parser );
		void		cmdPart( Client *client, Parser cmd );
};