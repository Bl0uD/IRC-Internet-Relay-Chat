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

	int id = FindClientId( ClientFd );
	std::string newNickname = Tokens[1];
	std::string oldNickname = this->_Clients[id].getNickname();

	if ( newNickname != oldNickname
		&& this->_Nicknames.find( newNickname ) != this->_Nicknames.end() )
	{
		this->SendToClient( ClientFd, ERR_NICKNAME_USED( newNickname ) );
		return ;
	}

	if ( oldNickname != "" )
		this->_Nicknames.erase( oldNickname );
	this->_Clients[id].setNickname( newNickname );
	this->_Nicknames.insert( newNickname );
	this->SendToClient( ClientFd, UPDATE_NICKNAME( this->_Clients[id].getNickname() ) );
}

void	Server::SetPassword( std::vector<std::string> Tokens, int ClientFd )
{
	if ( Tokens.size() != 2 )
	{
		this->SendToClient( ClientFd, ERR_CMD_ARGS( "PASS", "<your password>" ) );
		return ;
	}
	int id = FindClientId( ClientFd );

	this->_Clients[id].setPassword( Tokens[1] );
	this->SendToClient( ClientFd, UPDATE_PASSWORD );
	if ( !this->_Clients[id].getUsername().empty() )
		this->_Clients[id].setRegistered( true );
}


void	Server::ChangeTopic( std::vector<std::string> Tokens, int ClientFd )
{
	if ( Tokens.size() < 3 )
	{
		this->SendToClient( ClientFd, ERR_CMD_ARGS( "TOPIC", "<old topic> <new topic>" ) );
		return ;
	}

	std::string oldTopic = Tokens[1];
	int	ChannelId = FindChannelId( oldTopic );
	if ( ChannelId == -1 )
	{
		this->SendToClient( ClientFd, ERR_NOT_TOPIC_FOUND( oldTopic ) );
		return ;
	}

	std::string newTopic = Tokens[2];
	if ( FindDuplicateTopic( oldTopic, newTopic ) )
	{
		this->SendToClient( ClientFd, ERR_TOPIC_USED( newTopic ) );
		return ;
	}

	this->_Channels[ChannelId].setTopic( newTopic );
	this->SendToClient( ClientFd, NEW_TOPIC_SET( this->_Channels[ChannelId].getTopic() ) );
	return ;
}

void	Server::KickClient( std::vector<std::string> Tokens, int ClientFd )
{
	if ( Tokens.size() != 3 )
	{
		this->SendToClient( ClientFd, ERR_CMD_ARGS( "KICK", "<channel topic> <username>" ) );
		return ;
	}

	int channelId = FindChannelId(Tokens[1]);
	if ( channelId == -1 )
	{
		ERR_INEXISTANT_CHANNEL( Tokens[1] );
		return ;
	}

	int TargetFd = FindClientFd( Tokens[2] );
	if ( TargetFd == -1 )
	{
		ERR_INEXISTANT_CLIENT( Tokens[2] );
		return ;
	}
	
	if ( !IsOperator( ClientFd, channelId ))
	{
		ERR_NOT_OPERATOR();
		return ;
	}

	this->_Channels[channelId].removeClient( TargetFd );
	this->SendToClient( TargetFd, RECEIVED_KICK( this->_Channels[ channelId ].getTopic() ) );

	std::string	Topic = this->_Channels[channelId].getTopic();
	std::string	Nickname = this->_Clients[FindClientId( TargetFd )].getNickname();
	this->SendToAllMembers( channelId, CLIENT_KICKED( Topic, Nickname ) );
}

void	Server::InviteClient( std::vector<std::string> Tokens, int ClientFd )
{
	if ( Tokens.size() < 3 )
	{
		this->SendToClient( ClientFd, ERR_CMD_ARGS( "INVITE", "<channel name, client nickname>" ) );
		return ;
	}

	int channelId = FindChannelId( Tokens[1] );
	if ( channelId == -1 )
	{
		this->SendToClient( ClientFd, ERR_INEXISTANT_CHANNEL( Tokens[1] ) );
		return ;
	}

	if ( !IsOperator( ClientFd, channelId ) )
	{
		ERR_NOT_OPERATOR( Tokens[1] );
		return ;
	}

	int TargetFd = FindClientFd( Tokens[2] );
	if ( TargetFd == -1 )
		this->SendToClient( ClientFd, ERR_INEXISTANT_CLIENT( Tokens[2] ) );	

	this->_Channels[ channelId ].addPendingClient( TargetFd );
	this->SendToClient( TargetFd, RECEIVED_INVITE( this->_Channels[ channelId ].getTopic() ) );

	std::string Topic = this->_Channels[channelId].getTopic();
	std::string Nickname = this->_Clients[FindClientId( TargetFd )].getNickname();
	this->SendToAllMembers( channelId, CLIENT_INVITED( Topic, Nickname ) );
}
bool	Server::IsChannelJoinable( int	CliendFd, int ChannelId )
{
	if ( ChannelId < 0 || ChannelId >= static_cast<int>( this->_Channels.size() ) )
		return false;
	if ( this->_Channels[ ChannelId ].getInviteOnly()
		&& !this->_Channels[ ChannelId ].hasPendingClient( CliendFd ) )
		return false;
	if ( this->_Channels[ ChannelId ].getUserLimitation() > 0
		&& this->_Channels[ ChannelId ].getClients().size() >= static_cast<size_t>( this->_Channels[ ChannelId ].getUserLimitation() ) )
		return false;
	return true;
}

void	Server::JoinChannel( std::vector<std::string> Tokens, int ClientFd )
{
	if ( Tokens.size() < 2 )
	{
		this->SendToClient( ClientFd, ERR_CMD_ARGS( "JOIN", "<channel name>" ) );
		return ;
	}

	std::string newTopic = Tokens[1];
	int id = FindChannelId( newTopic );
	if ( id == -1 )
	{
		Channel newChannel;
		id = this->_NextChannelId++;
		newChannel.setId( id );
		newChannel.setTopic( newTopic );
		this->_Channels.push_back( newChannel );
		this->_ChannelDirectory.push_back( std::make_pair( id, newTopic ) );
		this->SendToClient( ClientFd, CREATE_CHANNEL( newTopic ) );
	}

	if ( this->_Channels[id].getClients().empty() )
	{
		this->_Channels[id].addOperator( ClientFd );
		this->_Channels[id].addClient( ClientFd );
		return ;
	}
	if ( IsChannelJoinable( ClientFd, id ))
	{
		this->_Channels[id].addClient( ClientFd );
		this->_Channels[id].removePendingClient( ClientFd );
	}
	else
		this->SendToClient( ClientFd, "Channel is invite-only or full." );
}

void	GiveTakeOperatorGrade( Channel *channel, int ClientFd )
{
	if ( channel == NULL )
		return ;

	if ( channel->getOperators().find( ClientFd ) != channel->getOperators().end() )
		channel->removeOperator( ClientFd );
	else
		channel->addOperator( ClientFd );
}

void	Server::ChangeMode( std::vector<std::string> Tokens, int ClientFd )
{
	if ( (Tokens.size() < 3 && Tokens.size() > 4) || Tokens[2].size() != 1 )
	{
		this->SendToClient( ClientFd, ERR_CMD_ARGS( "MODE", "<channel topic> <FLAG>" ) ); //flags: i t k o l
		return ;
	}
	int channelId = FindChannelId( Tokens[1] );
	if ( channelId == -1 )
	{
		SendToClient( ClientFd, ERR_INEXISTANT_CHANNEL( Tokens[1] ));
		return ;
	}
	
	char	flag = Tokens[2][0];
	switch( flag )
	{
		case 'i':
			this->_Channels[ channelId ].setInviteOnly();
			break;

		case 't':
			break;

		case 'k':
			if ( Tokens.size() != 4 )
			{
				this->SendToClient( ClientFd, ERR_CMD_ARGS( "MODE", "<channel topic> k <new password>" ) );
				return ;
			}
			std::cout << " fonction mode avec flag k appelle\n";
			//this->_Channels[ channelId ].addPassword( Tokens[4] );
			break;

		case 'o':
		{
			if ( Tokens.size() != 4 )
			{
				this->SendToClient( ClientFd, ERR_CMD_ARGS( "MODE", "<channel topic> o <nickname>" ) );
				return ;
			}
			int TargetFd = FindClientFd( Tokens[3] );
			if ( TargetFd == -1 )
			{
				ERR_INEXISTANT_CLIENT( Tokens[3] );
				return ;
			}
			this->_Channels[ channelId ].addOperator( TargetFd );
			break;
		}

		case 'l':
			if ( Tokens.size() != 4 )
			{
				this->SendToClient( ClientFd, ERR_CMD_ARGS( "MODE", "<channel topic> l <user limitation>" ) );
				return ;
			}
			GiveTakeOperatorGrade( &this->_Channels[ channelId ], ClientFd );
			break;

		default:
			this->SendToClient( ClientFd, HELP_MODE );
			break ;

	}
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

void	Server::ChannelMessage( std::vector< std::string > Tokens, int ClientFd )
{
	if ( Tokens.size() < 3 )
	{
		this->SendToClient( ClientFd, ERR_CMD_ARGS( "CHANNEL", "<channel topic> <message>" ) );
		return ;
	}

	int ChannelId = FindChannelId( Tokens[1] );
	if ( ChannelId == -1 )
	{
		this->SendToClient( ClientFd, ERR_INEXISTANT_CHANNEL( Tokens[1] ) );
		return ;
	}

	if ( !InChannel( ClientFd, ChannelId ) )
	{
		ERR_NOT_IN_CHANNEL( Tokens[1] );
		return ;
	}

	std::string message;
	for ( size_t i = 2; i < Tokens.size(); ++i )
	{
		if ( i > 2 )
			message += " ";
		message += Tokens[i];
	}

	SendToChannel( ClientFd, ChannelId, message);
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
	std::string	cmds[13] = {	"USER",				//0
								"NICK",				//1
								"PASS",				//2
								"TOPIC",			//3
								"KICK",				//4
								"INVITE",			//5
								"JOIN",				//6
								"MODE",				//7
								"PRIVMSG",			//8
								"HELP",				//9
								"CHANNEL",			//10
								"HELP_MODE",		//11
								"CHANNEL_ON" };		//12
	if ( Tokens.empty() )
		return ;
	while ( i < 13 && Tokens[0] != cmds[i] )
		i++;
	switch ( i )
	{
		case 0:		//USER
			SetUsername( Tokens, ClientFd );
			break ;
		case 1:		//NICK
			if ( IsRegistered( ClientFd ) )
				SetNickname( Tokens, ClientFd );
			break ;
		case 2:		//PASS
			SetPassword( Tokens, ClientFd );
			break ;
		case 3:		//TOPIC
			if ( IsRegistered( ClientFd ) )
				ChangeTopic( Tokens, ClientFd );
			break ;
		case 4:		//KICK
			if ( IsRegistered( ClientFd ) )
				KickClient( Tokens, ClientFd );
			break ;
		case 5:		//INVITE
			if ( IsRegistered( ClientFd ) )
				InviteClient( Tokens, ClientFd );
			break ;
		case 6:		//JOIN
			if ( IsRegistered( ClientFd ) )
				JoinChannel( Tokens, ClientFd );
			break ;
		case 7:		//MODE
			if ( IsRegistered( ClientFd ) )
				ChangeMode( Tokens, ClientFd );
			break ;
		case 8:		//PRIVMSG
			if ( IsRegistered( ClientFd ) )
				SendPrivMsg( Tokens, ClientFd );
			break ;
		case 9:		//HELP
			this->SendToClient( ClientFd, COMMAND_LIST );
			break ;
		case 10:	//CHANNEL
			if ( IsRegistered( ClientFd ) )
				this->ChannelMessage( Tokens, ClientFd );
			break ;
		case 11:	//HELP_MODE
			this->SendToClient( ClientFd, HELP_MODE );
			break ;
		case 12:	//CHANNEL_ON
			ChannelList( ClientFd );
			break ;
		default:
			this->SendToClient( ClientFd, ERR_CMD_NOT_FOUND( Tokens[0] ) );
			break ;
	}
}