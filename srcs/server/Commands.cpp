# include "../../includes/Server.hpp"

void	Server::SetUsername( std::vector<std::string> Tokens, int ClientFd )
{
	if ( Tokens.size() != 2 )
	{
		this->SendToClient( ClientFd, ERR_CMD_ARGS( "USER", "<your username>" ) );
		return ;
	}
	int i = FindClientId( ClientFd );
	this->_Clients[i].setUsername( Tokens[1] );
	this->SendToClient( ClientFd, UPDATE_USERNAME( this->_Clients[i].getUsername() ) );
	if ( !this->_Clients[i].getPassword().empty() )
		this->_Clients[i].setRegistered( true );
}

void	Server::SetNickname(  std::vector<std::string> Tokens, int ClientFd )
{
	if ( Tokens.size() != 2 )
	{
		this->SendToClient( ClientFd, ERR_CMD_ARGS( "NICK", "<your nickname>" ) );
		return ;
	}

	int i = FindClientId( ClientFd );
	if ( i < 0 )
		return ;

	std::string newNickname = Tokens[1];
	std::string oldNickname = this->_Clients[i].getNickname();

	if ( newNickname != oldNickname
		&& this->_Nicknames.find( newNickname ) != this->_Nicknames.end() )
	{
		this->SendToClient( ClientFd, ERR_NICKNAME_USED( newNickname ) );
		return ;
	}

	if ( oldNickname != "" )
		this->_Nicknames.erase( oldNickname );
	this->_Clients[i].setNickname( newNickname );
	this->_Nicknames.insert( newNickname );
	this->SendToClient( ClientFd, UPDATE_NICKNAME( this->_Clients[i].getNickname() ) );
}

void	Server::SetPassword( std::vector<std::string> Tokens, int ClientFd )
{
	if ( Tokens.size() != 2 )
	{
		this->SendToClient( ClientFd, ERR_CMD_ARGS( "PASS", "<your password>" ) );
		return ;
	}
	int i = FindClientId( ClientFd );
	this->_Clients[i].setPassword( Tokens[1] );
	this->SendToClient( ClientFd, UPDATE_PASSWORD );
	if ( !this->_Clients[i].getUsername().empty() )
		this->_Clients[i].setRegistered( true );
}


void	Server::ChangeTopic( std::vector<std::string> Tokens, int ClientFd )
{
	int i = FindClientId( ClientFd );
	if ( !this->_Clients[i].getRegistered() )
	{
		this->SendToClient( ClientFd, "Register first." );
		return ;
	}
	if ( Tokens.size() < 3 )
	{
		this->SendToClient( ClientFd, ERR_CMD_ARGS( "TOPIC", "<channel name> <new topic>" ) );
		return ;
	}

	std::string channelName = Tokens[1];
	std::string newTopic;
	int channelId = -1;

	for ( size_t i = 2; i < Tokens.size(); ++i )
	{
		if ( i > 2 )
			newTopic += " ";
		newTopic += Tokens[i];
	}
	if ( newTopic.size() > 0 && newTopic[0] == ':' )
		newTopic.erase( 0, 1 );

	for ( std::vector< std::pair<int, std::string> >::iterator it = this->_ChannelDirectory.begin(); it != this->_ChannelDirectory.end(); ++it )
	{
		if ( it->second == channelName )
		{
			channelId = it->first;
			break ;
		}
	}

	if ( channelId == -1 )
	{
		this->SendToClient( ClientFd, std::string( ":ircserv 403 " ) + channelName + " :No such channel" );
		return ;
	}

	for ( std::vector< Channel >::iterator it = this->_Channels.begin(); it != this->_Channels.end(); ++it )
	{
		if ( it->getId() == channelId )
		{
			it->setTopic( newTopic );
			this->SendToClient( ClientFd, std::string( ":ircserv NOTICE " ) + channelName + " :Topic updated" );
			return ;
		}
	}
}

void	Server::KickClient( std::vector<std::string> Tokens, int ClientFd )
{
	if ( !IsRegistered( ClientFd ) )
		return ;

	if ( Tokens.size() != 3 )
	{
		this->SendToClient( ClientFd, ERR_CMD_ARGS( "KICK", "<channel topic> <username>" ) );
		return ;
	}

	std::string	channelTopic = Tokens[1];
	int channelId = FindChannelId(channelTopic);
	if ( channelId == -1 )
		ERR_INEXISTANT_CHANNEL( channelTopic );
	int TargetFd = FindClientFd( Tokens[2] );
	if ( TargetFd == -1 )
	{
		ERR_INEXISTANT_CLIENT( Tokens[2] );
		return ;
	}
	
	for ( std::vector< Channel >::iterator it = this->_Channels.begin(); it != this->_Channels.end(); ++it )
	{
		if ( it->getTopic() == channelTopic )
		{
			it->removeClient( TargetFd );
			std::cout << "func Join" << std::endl;
			return ;
		}
	}
	std::string message = RED + "Client named ";
	int i = FindClientId( TargetFd );
	message += GREEN + this->_Clients[i].getNickname();
	message += "has been kicked from the channel\n" + WHITE;
	this->SendToAllMembers( channelId, message );
}

void	Server::InviteClient( std::vector<std::string> Tokens, int ClientFd )
{
	if ( Tokens.size() < 3 )
	{
		this->SendToClient( ClientFd, ERR_CMD_ARGS( "INVITE", "<channel name, client nickname>" ) );
		return ;
	}

	std::string channelName = Tokens[1];
	int channelId = FindChannelId( channelName );
	if ( channelId == -1 )
	{
		this->SendToClient( ClientFd, ERR_INEXISTANT_CHANNEL( channelName ) );
		return ;
	}
	if ( !IsOperator( ClientFd, channelId ) )
	{
		ERR_NOT_OPERATOR( channelName );
		return ;
	}
	int TargetFd = FindClientFd( Tokens[2] );
	if ( TargetFd == -1 )
		this->SendToClient( ClientFd, ERR_INEXISTANT_CLIENT( Tokens[2] ) );	

	this->_Channels[ channelId ].addPendingClient( TargetFd );
	this->SendToClient( TargetFd, CHANNEL_INVITE( this->_Channels[ channelId ].getTopic() ) );

	std::string message = RED + "Client named ";
	int i = FindClientId( TargetFd );
	message += GREEN + this->_Clients[i].getNickname();
	message += "has been invited in the channel\n" + WHITE;
	this->SendToAllMembers( channelId, message );

}

void	Server::JoinChannel( std::vector<std::string> Tokens, int ClientFd )
{
	if ( Tokens.size() < 2 )
	{
		this->SendToClient( ClientFd, ERR_CMD_ARGS( "JOIN", "<channel name>" ) );
		return ;
	}

	std::string channelName = Tokens[1];
	int channelId = FindChannelId( channelName );
	std::cout << "channelId : " << channelId << std::endl;
	if ( channelId == -1 )
	{
		Channel newChannel;

		channelId = this->_NextChannelId++;
		newChannel.setId( channelId );
		newChannel.setTopic( channelName );
		this->_Channels.push_back( newChannel );
		this->_ChannelDirectory.push_back( std::make_pair( channelId, channelName ) );
		this->SendToClient( ClientFd, CREATE_CHANNEL( channelName ) );
	}

	for ( std::vector< Channel >::iterator it = this->_Channels.begin(); it != this->_Channels.end(); ++it )
	{
		if ( it->getId() == channelId )
		{
			if ( it->getClients().empty() )
				it->addOperator( ClientFd );
			it->addClient( ClientFd );
			std::cout << "func Join" << std::endl;
			return ;
		}
	}
}

void	Server::ChangeMode( std::vector<std::string> Tokens, int ClientFd )
{
	if ( Tokens.size() != 2 )
	{
		this->SendToClient( ClientFd, ERR_CMD_ARGS( "MODE", "<channel topic> <ARGS>" ) );
		return ;
	}
	int channelId = FindChannelId( Tokens[1] );
	if ( channelId == -1 )
	{
		SendToClient( ClientFd, ERR_INEXISTANT_CHANNEL( Tokens[1] ));
		return ;
	}
	this->_Channels[ channelId ].addOperator( ClientFd );
	this->SendToClient( ClientFd, CHANNEL_OPERATOR( this->_Channels[ channelId ].getTopic() ));

}

void	Server::SendPrivMsg( std::vector<std::string> Tokens, int ClientFd )
{
	if ( Tokens.size() < 3 )
	{
		this->SendToClient( ClientFd, ERR_CMD_ARGS( "PRIVMSG", "<nickname> <message>" ) );
		return ;
	}

	std::string targetNickname = Tokens[1];
	std::string message;
	std::string senderNickname;
	int senderIndex = FindClientId( ClientFd );
	int targetFd = -1;

	if ( senderIndex >= 0 && senderIndex < static_cast<int>( this->_Clients.size() ) )
	{
		senderNickname = this->_Clients[senderIndex].getNickname();
		if ( senderNickname == "" )
			senderNickname = this->_Clients[senderIndex].getUsername();
	}

	targetFd = FindClientFd( targetNickname );
	if ( targetFd == -1 )
	{
		this->SendToClient( ClientFd, std::string( ":ircserv 401 " ) + targetNickname + " :No such nick/user" );
		return ;
	}

	for ( size_t i = 2; i < Tokens.size(); ++i )
	{
		if ( i > 2 )
			message += " ";
		message += Tokens[i];
	}
	
	this->SendToClient( targetFd, std::string( ":" ) + senderNickname + " PRIVMSG " + targetNickname + " :" + message );
}

void	Server::ChannelList( int ClientFd )
{
	if ( this->_Channels.size() == 0 )
	{
		this->SendToClient( ClientFd, "No such channel existing..");
		return ;
	}

	std::string list = "";
	
	for ( size_t i = 0; i < this->_Channels.size(); i++ )
	{
		list += this->_Channels[i].getTopic();
		if ( i + 1 < this->_Channels.size())
			list += std::string("\n");
	}

	SendToClient( ClientFd, list );
}

void	Server::ExecCommand( std::vector<std::string> Tokens, int ClientFd )
{
	int			i = 0;
	std::string	cmds[13] = {	"USER",
								"NICK",
								"PASS",
								"TOPIC",
								"KICK",
								"INVITE",
								"JOIN",
								"MODE",
								"PRIVMSG",
								"HELP",
								"CHANNEL",
								"HELP_MODE",
								"CHANNEL_ON" };
	if ( Tokens.empty() )
		return ;
	while ( i < 13 && Tokens[0] != cmds[i] )
		i++;
	switch ( i )
	{
		case 0:
			SetUsername( Tokens, ClientFd );
			break ;
		case 1:
			if ( IsRegistered( ClientFd ) )
				SetNickname( Tokens, ClientFd );
			break ;
		case 2:
			SetPassword( Tokens, ClientFd );
			break ;
		case 3:
			if ( IsRegistered( ClientFd ) )
				ChangeTopic( Tokens, ClientFd );
			break ;
		case 4:
			if ( IsRegistered( ClientFd ) )
				KickClient( Tokens, ClientFd );
			break ;
		case 5:
			if ( IsRegistered( ClientFd ) )
				InviteClient( Tokens, ClientFd );
			break ;
		case 6:	
			if ( IsRegistered( ClientFd ) )
				JoinChannel( Tokens, ClientFd );
			break ;
		case 7:
			if ( IsRegistered( ClientFd ) )
				ChangeMode( Tokens, ClientFd );
			break ;
		case 8:
			if ( IsRegistered( ClientFd ) )
				SendPrivMsg( Tokens, ClientFd );
			break ;
		case 9:
			this->SendToClient( ClientFd, COMMAND_LIST );
			break ;
		case 10:
			if ( IsRegistered( ClientFd ) )
				this->SendToChannel( ClientFd, Tokens );
			break ;
		case 11:
			this->SendToClient( ClientFd, HELP_MODE );
			break ;
		case 12:
			ChannelList( ClientFd );
			break ;
		default:
			this->SendToClient( ClientFd, ERR_CMD_NOT_FOUND( Tokens[0] ) );
			break ;
	}
}